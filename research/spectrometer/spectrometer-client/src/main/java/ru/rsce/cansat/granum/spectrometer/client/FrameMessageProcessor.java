package ru.rsce.cansat.granum.spectrometer.client;

import java.awt.EventQueue;
import java.awt.Point;
import java.awt.image.BufferedImage;
import java.awt.image.DataBufferByte;
import java.awt.image.Raster;
import java.security.InvalidParameterException;
import java.util.ArrayList;

import org.apache.commons.cli.ParseException;
import org.jfree.data.xy.XYDataItem;

import ru.rsce.cansat.granum.spectrometer.client.gui.MainWindow;
import ru.rsce.cansat.granum.spectrometer.client.netty.FrameMessage;
import ru.rsce.cansat.granum.spectrometer.client.netty.Message;
import ru.rsce.cansat.granum.spectrometer.client.netty.SpectrometerClient;
import ru.rsce.cansat.granum.spectrometer.client.netty.SpectrometerClient.ClientMessageListener;

public class FrameMessageProcessor implements ClientMessageListener {
	
	public enum ImageColorMode {
		COLORFULL, GRAYSCALE;
		
		static ImageColorMode fromString(String value) throws ParseException {
			if (value.equals("colorfull"))
				return COLORFULL;
			else if (value.equals("grayscale"))
				return GRAYSCALE;

			throw new ParseException(String.format("Не верный цветовой режим: \"%s\"", value));
		}
	};
	

	public void attachToSpectrometerClient(SpectrometerClient client) {
		client.setMsgListener(this);
	}
	
	@Override
	public void onSpectrometerMessage(Message msg_) throws Exception {
		if (msg_.getType() != Message.Type.RAW_FRAME)
			throw new InvalidParameterException("не поддерживаемый тип сообщения!");
		
		FrameMessage msg = (FrameMessage)msg_;
		final BufferedImage picture = convertImage(msg);
		final ArrayList<XYDataItem> plotData = calculatePlotData(msg);
		final MainWindow _mw = mw;
		
		EventQueue.invokeAndWait(new Runnable() {
			
			@Override
			public void run() {
				_mw.setSpectroPicture(picture);
				_mw.setSpectroPlotData(plotData);
			}
		});
	}
	
	
	public void attachToMainWindow(MainWindow mw_) {
		mw = mw_;
	}
	

	public void setImageColorMode(ImageColorMode mode_) {
		colorMode = mode_;
	}
	
		
	public void setScanlineParams(int xCenter_, int xWidth_, int yStart_, int yStop_) {

		if (xWidth_ < 0)
			throw new InvalidParameterException("Не верное значение xWidth : < 0");
		
		if (xCenter_ < 0)
			throw new InvalidParameterException("Не верное значение xCenter : < 0");
		
		if (xCenter_ - xWidth_ / 2 < 0)
			throw new InvalidParameterException("Не верное значение xCenter - xWidth/2 : < 0");
		
		if (yStart_ < 0 || yStop_ < 0)
			throw new InvalidParameterException("Не верное значение yStart или yStop : < 0");
		
		yStart = Math.min(yStart_, yStop_);
		yStop = Math.max(yStart_, yStop_);
		xCenter = xCenter_;
		xWidth = xWidth_;
		
		if (mw != null)
			mw.setScanRegion(xCenter - xWidth/2, yStart, xWidth, yStop-yStart);
	}
	
	
	protected BufferedImage convertImage(FrameMessage message) {
	
		final int bufferedImageType;
		final byte[] rawData;
		
		switch (message.getPixFmt()) {
		case YUYV422:
			
			if (colorMode == ImageColorMode.COLORFULL) {
				bufferedImageType = BufferedImage.TYPE_3BYTE_BGR;
				rawData = _bgr888_from_YUYV(message.getPixData());
			} else {
				bufferedImageType = BufferedImage.TYPE_BYTE_GRAY;
				rawData = _i8_from_YUYV(message.getPixData());
			}
			break;
		default:
			throw new InvalidParameterException("Не могут работать данными форматом пикселей кадра");
		}
		
		BufferedImage retval = new BufferedImage(message.getWidth(), message.getHeight(), bufferedImageType);
		retval.setData(Raster.createRaster(retval.getSampleModel(),
				new DataBufferByte(rawData, rawData.length), new Point()));
		
		return retval;
	}
	
	
	protected ArrayList<XYDataItem> calculatePlotData(FrameMessage message) {

		assert xWidth >= 0;
		final int xLeftBound;
		final int xRightBound;
		if (xWidth == 1) { 
			xLeftBound = xCenter;
			xRightBound = xCenter + 1;
		} else {
			xLeftBound = xCenter - xWidth / 2;
			xRightBound = xCenter + xWidth / 2;
		}
		
		assert yStart <= yStop;
		final int yTopBound = yStart;
		final int yBottomBound = yStop;
		
		ArrayList<XYDataItem> dataString = new ArrayList<>();
		dataString.ensureCapacity((yStop-yStart));
		
		for (int y = yTopBound; y < yBottomBound; y++) {
			int accamulatedData = 0;
			for (int x = xLeftBound; x < xRightBound; x++) {
				int targetPixelNumber = message.getWidth() * y + x;
				int targetPixelYData = message.getPixData()[targetPixelNumber * 2] & 0xFF;
				accamulatedData += targetPixelYData;
			}
			dataString.add(new XYDataItem(y, accamulatedData));
		}
		
		return dataString;
	}

	
	private byte[] _bgr888_from_YUYV(byte[] input) {
		byte[] output = new byte[input.length / 2 * 3];
		
		for (int pixId = 0; pixId < input.length / 4; pixId++) {
			final int inpOffset = pixId*2*2;
			float y1  = input[inpOffset+0] & 0xFF;
			float cb  = input[inpOffset+1] & 0xFF;
			float y2  = input[inpOffset+2] & 0xFF;
			float cr  = input[inpOffset+3] & 0xFF;
			
			float r1 = y1 + (1.4065f * (cr - 128f));
			float g1 = y1 - (0.3455f * (cb - 128f)) - (0.7169f * (cr - 128f));
			float b1 = y1 + (1.7790f * (cb - 128f));
			
			float r2 = y2 + (1.4065f * (cr - 128f));
			float g2 = y2 - (0.3455f * (cb - 128f)) - (0.7169f * (cr - 128f));
			float b2 = y2 + (1.7790f * (cb - 128f));
			
			r1 = Math.max(Math.min(r1, 255f), 0f);
			g1 = Math.max(Math.min(g1, 255f), 0f);
			b1 = Math.max(Math.min(b1, 255f), 0f);
			r2 = Math.max(Math.min(r2, 255f), 0f);
			g2 = Math.max(Math.min(g2, 255f), 0f);
			b2 = Math.max(Math.min(b2, 255f), 0f);
			
			final int outputOffset = pixId*3*2;
			output[outputOffset+0] = (byte)(b1);
			output[outputOffset+1] = (byte)(g1);
			output[outputOffset+2] = (byte)(r1);
			
			output[outputOffset+3] = (byte)(b2);
			output[outputOffset+4] = (byte)(g2);
			output[outputOffset+5] = (byte)(r2);
		}
		
		return output;
	}
	
	
	private byte[] _i8_from_YUYV(byte[] yuyvData) {  
		byte[] retval = new byte[yuyvData.length / 2];
		
 		for (int i = 0; i < retval.length; i++) {
			retval[i] = yuyvData[i*2];
		}
		
		return retval;
	}
	

	private MainWindow mw;
	
	private ImageColorMode colorMode;
	private int xCenter;
	private int xWidth;
	
	private int yStart;
	private int yStop;
}

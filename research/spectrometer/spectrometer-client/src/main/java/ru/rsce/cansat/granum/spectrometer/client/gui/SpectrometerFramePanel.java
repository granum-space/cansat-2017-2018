package ru.rsce.cansat.granum.spectrometer.client.gui;

import java.awt.Graphics;
import java.awt.Point;
import java.awt.image.BufferedImage;
import java.awt.image.DataBufferByte;
import java.awt.image.Raster;
import java.security.InvalidParameterException;

import javax.swing.JPanel;

import ru.rsce.cansat.granum.spectrometer.client.netty.FrameMessage;

public class SpectrometerFramePanel extends JPanel {
	
	private static final long serialVersionUID = 1L;

	
	public void updateFrame(FrameMessage msg) {
		byte[] rawData;
		int bufferedImageType = BufferedImage.TYPE_3BYTE_BGR;
		
		switch (msg.getPixFmt()) {
		case YUYV422:
			rawData = _bgr888_from_YUYV(msg.getPixData());
			break;
		default:
			throw new InvalidParameterException("Не могут работать данными форматом пикселей кадра");
		}
		
		BufferedImage retval = new BufferedImage(msg.getWidth(), msg.getHeight(), bufferedImageType);
		retval.setData(Raster.createRaster(retval.getSampleModel(),
				new DataBufferByte(rawData, rawData.length), new Point() ) );
						
		frame = retval;
	}
	
	
	@Override
	protected void paintComponent(Graphics g) {
		super.paintComponent(g);
		
		if (frame == null)
			return;
		
		g.drawImage(frame, 0, 0, this.getWidth(), this.getHeight(), this);
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
			output[outputOffset+0] = (byte)(r1);
			output[outputOffset+1] = (byte)(g1);
			output[outputOffset+2] = (byte)(b1);
			
			output[outputOffset+3] = (byte)(r2);
			output[outputOffset+4] = (byte)(g2);
			output[outputOffset+5] = (byte)(b2);
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

	private BufferedImage frame;
}

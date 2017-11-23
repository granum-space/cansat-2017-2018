package ru.rsce.cansat.granum.spectrometer.client.netty;

import io.netty.buffer.ByteBuf;

public class FrameMessage extends Message {
	
	public static enum PixFmt
	{
		YUYV422;
		
		public static long fourccFromString(String code) {
			byte[] raw = code.getBytes();
			assert raw.length == 4;
			
			return (raw[0] << 0*8) | (raw[1] << 1*8) | (raw[2] << 2*8) | (raw[3] << 3*8);
		}

		public static PixFmt fromUint(long value) throws IllegalArgumentException {
			if (fourccFromString("YUYV") == value)
				return YUYV422;
		
			throw new IllegalArgumentException("Не верный fourcc код типа данных кадра");
		}
	}
	

	protected FrameMessage(int size_, Type type_, ByteBuf rawData) throws IllegalArgumentException {
		super(size_, type_);
		
		assert this.getType() == Type.RAW_FRAME;
		assert rawData.isReadable(headerSize);
		
		try {
			width = rawData.readIntLE();
			height = rawData.readIntLE();
			pixFmt = PixFmt.fromUint(rawData.readUnsignedIntLE());
			
			int pixDataSize = (int)rawData.readUnsignedIntLE();
			assert pixDataSize == this.getPayloadSize() - headerSize;
			
			pixData = new byte[pixDataSize];
			rawData.readBytes(pixData);
			
		} catch (Exception e){
			rawData.resetReaderIndex();
			throw e;
		}
	}
	
	
	public int getWidth() {
		return width;
	}
	
	
	public int getHeight() {
		return height;
	}
	
	
	public PixFmt getPixFmt() {
		return pixFmt;
	}
	
	public byte[] getPixData() {
		return pixData;
	}
		
	
	private int width;
	private int height;
	private PixFmt pixFmt;
	//private long pixDataSize;
	private byte[] pixData;
	
	
	private static final int headerSize = 
			Integer.BYTES +	// int32_t width
			Integer.BYTES +	// int32_t height
			Integer.BYTES +	// int32_t v4l2_fourcc
			Integer.BYTES   // int32_t size;
			;
}

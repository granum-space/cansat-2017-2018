package ru.rsce.cansat.granum.spectrometer.client.netty;

public class Message {
	
	public static enum Type {
		RAW_FRAME;

		public static Type fromInt(int value) throws IllegalArgumentException {
			if (0 == value) {
				return Type.RAW_FRAME;
			}

			throw new IllegalArgumentException("Не верный тип сообщения");
		}
	}
	
	
	protected Message(int size_, Type type_) throws IllegalArgumentException	{
		size = size_;
		type = type_;
	}

	
	public Type getType() {
		return type;
	}
	
	
	public int getSize() {
		return size;
	}
	
	
	public int getPayloadSize() {
		return size - headerSize;
	}
	
	
	private Type type;
	private int size;
	
	
	public static final int headerSize = 
			Integer.BYTES + // int32_t message_size;
			Integer.BYTES   // int32_t message_type;
			;
}

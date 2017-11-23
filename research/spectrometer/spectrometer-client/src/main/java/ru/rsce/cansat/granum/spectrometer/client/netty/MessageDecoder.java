package ru.rsce.cansat.granum.spectrometer.client.netty;

import java.util.List;

import io.netty.buffer.ByteBuf;
import io.netty.channel.ChannelHandlerContext;
import io.netty.handler.codec.ByteToMessageDecoder;

import ru.rsce.cansat.granum.spectrometer.client.netty.FrameMessage;

public class MessageDecoder extends ByteToMessageDecoder {

	@Override
	protected void decode(ChannelHandlerContext ctx, ByteBuf in, List<Object> out) throws Exception {
		
		if (!in.isReadable(Message.headerSize))
			return;
		
		in.markReaderIndex();
		int messageSize = in.readIntLE();
		
		if (!in.isReadable(messageSize - Integer.BYTES)) {
			in.resetReaderIndex();
			return;
		}
		
		try {
		
			Message.Type messageType = Message.Type.fromInt(in.readIntLE());
			switch (messageType) {
			case RAW_FRAME:
					out.add(new FrameMessage(messageSize, messageType, in));
					break;
			};
		} catch (Exception e) {
			ctx.channel().disconnect();
			throw e;
		}
	}
}


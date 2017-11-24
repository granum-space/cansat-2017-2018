package ru.rsce.cansat.granum.spectrometer.client.netty;

import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;

import ru.rsce.cansat.granum.spectrometer.client.netty.FrameMessage;

public class MessageHandler extends SimpleChannelInboundHandler<FrameMessage> {
	
	MessageHandler(SpectrometerClient parent_) {
		parent = parent_;
	}
	

	@Override
	protected void channelRead0(ChannelHandlerContext ctx, FrameMessage msg) throws Exception {
		System.out.println(String.format("got message! Size = %d", msg.getSize()));
		
		parent.pushMessage(msg);
	}
	
	private final SpectrometerClient parent;
}

package ru.rsce.cansat.granum.spectrometer.client;

import java.awt.EventQueue;

import ru.rsce.cansat.granum.spectrometer.client.gui.MainWindow;
import ru.rsce.cansat.granum.spectrometer.client.netty.Message;
import ru.rsce.cansat.granum.spectrometer.client.netty.SpectrometerClient;

public class Main {

	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			@Override
			public void run() {
				realMain(args);
			}
		});


	}

	
	private static void realMain(String[] args) {
		final MainWindow window;
		final SpectrometerClient client;
		
		try {
			client = new SpectrometerClient();
			window = new MainWindow();
			
			client.setMsgListener(new SpectrometerClient.MessageListener() {

				@Override
				public void onMessage(Message msg_) throws Exception {
					final Message msg = msg_;
				
					EventQueue.invokeAndWait(new Runnable() {

						@Override
						public void run() {
							try {
								window.processSpectrometerMessage(msg);
							} catch (Exception e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							}
						}
						
					});
				}
			});
			
			
			client.start(args[0], Integer.parseInt(args[1]));
			
		} catch (Exception e) {
			e.printStackTrace();
		}

		//window.frame.setVisible(true);
	}
}

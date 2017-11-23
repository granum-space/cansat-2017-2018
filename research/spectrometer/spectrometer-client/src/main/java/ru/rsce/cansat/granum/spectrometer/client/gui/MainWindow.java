package ru.rsce.cansat.granum.spectrometer.client.gui;

import java.awt.EventQueue;

import javax.swing.JFrame;
import javax.swing.JPanel;

import ru.rsce.cansat.granum.spectrometer.client.netty.FrameMessage;
import ru.rsce.cansat.granum.spectrometer.client.netty.Message;
import ru.rsce.cansat.granum.spectrometer.client.netty.Server;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.Point;
import java.awt.image.BufferedImage;
import java.awt.image.DataBufferByte;
import java.awt.image.Raster;
import java.io.IOException;

public class MainWindow {

	private JFrame frame;

	/**
	 */
	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					MainWindow window = new MainWindow();
					window.frame.setVisible(true);
					
					Server s = new Server();
					s.start(args[0], Integer.parseInt(args[1]));

					s.setMsgListener(new Server.MessageListener() {
						
						@Override
						public void onMessage(Message msg_) {
							FrameMessage msg = (FrameMessage)msg_;
							
							byte[] pixData = msg.getPixData();
							byte[] ybytes = new byte[pixData.length / 2];
							for (int i = 0; i < ybytes.length; i++) {
								ybytes[i] = pixData[2*i];
							}
							
							BufferedImage img = new BufferedImage(msg.getWidth(), msg.getHeight(),
									BufferedImage.TYPE_BYTE_GRAY);
							img.setData(Raster.createRaster(img.getSampleModel(),
									new DataBufferByte(ybytes, ybytes.length), new Point() ) );
								
							window.picturePanel.setImage(img);
							window.picturePanel.repaint();
						}
					});
					
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}

	/**
	 * Create the application.
	 */
	public MainWindow() throws IOException {
		initialize();
	}

	/**
	 * Initialize the contents of the frame.
	 * @throws IOException 
	 */
	private void initialize() throws IOException {
		frame = new JFrame();
		frame.setBounds(100, 100, 450, 300);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.getContentPane().setLayout(new GridLayout(0, 1, 0, 0));
		
		picturePanel = new PicturePanel();
		frame.getContentPane().add(picturePanel);
	}
	
	
	private PicturePanel picturePanel;

}

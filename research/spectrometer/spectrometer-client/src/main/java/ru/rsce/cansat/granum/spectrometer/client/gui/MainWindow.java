package ru.rsce.cansat.granum.spectrometer.client.gui;

import javax.swing.JFrame;

import ru.rsce.cansat.granum.spectrometer.client.netty.FrameMessage;
import ru.rsce.cansat.granum.spectrometer.client.netty.Message;

import java.awt.GridLayout;

public class MainWindow {

	/**
	 * Create the application.
	 */
	public MainWindow() {
		initialize();
	}

	/**
	 * Initialize the contents of the frame.
	 * @throws IOException 
	 */
	private void initialize() {
		frame = new JFrame();
		frame.setBounds(100, 100, 450, 300);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.getContentPane().setLayout(new GridLayout(0, 1, 0, 0));
		
		picturePanel = new SpectrometerFramePanel();
		frame.getContentPane().add(picturePanel);
		frame.setVisible(true);
	}
	
	
	public void processSpectrometerMessage(Message msg_) throws Exception{
		if (msg_.getType() != Message.Type.RAW_FRAME)
			throw new Exception("Не верный тип сообщений");
		
		FrameMessage msg = (FrameMessage)(msg_);

		picturePanel.updateFrame(msg);
		picturePanel.repaint();
	}
	
	
	private JFrame frame;
	private SpectrometerFramePanel picturePanel;
}

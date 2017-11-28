package ru.rsce.cansat.granum.spectrometer.client.gui;

import javax.swing.JSplitPane;
import javax.swing.JFrame;
import javax.swing.JPanel;

import java.awt.BorderLayout;
import java.awt.image.BufferedImage;

import org.jfree.data.xy.XYDataItem;
import org.jfree.data.xy.XYSeries;
import java.util.List;


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
		mainFrame = new JFrame();
		mainFrame.setBounds(100, 100, 450, 300);
		mainFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	
		JPanel panel = new JPanel();
		panel.setLayout(new BorderLayout(0, 0));
		JSplitPane splitPane = new JSplitPane();
		splitPane.setOrientation(JSplitPane.VERTICAL_SPLIT);
		splitPane.setResizeWeight(0.5);
		splitPane.setDividerLocation(0.5);
			
		spectrometerPicturePanel = new SpectrometerPicturePanel();
		spectrometerPicturePanel.setLayout(new BorderLayout(0, 0));
		
		spectrometerPlotPanel = new SpectrometerPlotPanel();

		splitPane.setLeftComponent(spectrometerPicturePanel);
		splitPane.setRightComponent(spectrometerPlotPanel);
		panel.add(splitPane);
		mainFrame.getContentPane().add(panel);
	}
	
	
	public void setSpectroPicture(BufferedImage picture) {
		spectrometerPicturePanel.setSpectroPicture(picture);
		spectrometerPicturePanel.repaint();
	}
	
	
	public void setScanRegion(int xleft, int ytop, int width, int height) {
		spectrometerPicturePanel.setScanRect(xleft, ytop, width, height);
	}
	
	
	public void setSpectroPlotData(List<XYDataItem> plotData) {
		XYSeries series = new XYSeries("Интенсивность");
		for (XYDataItem item: plotData) {
			series.add(item);
		}
		
		spectrometerPlotPanel.setPlotData(series);
	}
	
	public void show() {
		mainFrame.setVisible(true);
	}
	
	private JFrame mainFrame;
	private SpectrometerPicturePanel spectrometerPicturePanel;
	private SpectrometerPlotPanel spectrometerPlotPanel;
}

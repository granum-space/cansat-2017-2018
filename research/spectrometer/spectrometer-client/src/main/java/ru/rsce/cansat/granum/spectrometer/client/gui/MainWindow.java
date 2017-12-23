package ru.rsce.cansat.granum.spectrometer.client.gui;

import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.util.List;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JSplitPane;

import org.jfree.data.xy.XYDataItem;
import org.jfree.data.xy.XYSeries;

import ru.rsce.cansat.granum.spectrometer.client.gui.icons.TextIcon;


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
	
		JPanel rootPanel = new JPanel();
		rootPanel.setLayout(new BorderLayout(0, 0));
		
		JSplitPane splitPane = new JSplitPane();
		splitPane.setOrientation(JSplitPane.VERTICAL_SPLIT);
		splitPane.setResizeWeight(0.5);
		splitPane.setDividerLocation(0.5);
		
                JSplitPane upperSplit = new JSplitPane();
                upperSplit.setOrientation(JSplitPane.HORIZONTAL_SPLIT);
                upperSplit.setResizeWeight(1);
                upperSplit.setDividerLocation(0.8);
                
                spectrometerPicturePanel = new SpectrometerPicturePanel();
		spectrometerPicturePanel.setLayout(new BorderLayout(0, 0));
                upperSplit.setLeftComponent(spectrometerPicturePanel);
                
                spectrometerSliderPanel = new SpectrometerSliderPanel();
                upperSplit.setRightComponent(spectrometerSliderPanel);
                
                splitPane.setLeftComponent(upperSplit);
                
		plotAndControlsPanel = new JPanel();
                
		spectrometerPlotPanel = new SpectrometerPlotPanel();

		plotAndControlsPanel.setLayout(new BorderLayout(0, 0));
		
		plotAndControlsPanel.add(spectrometerPlotPanel, BorderLayout.CENTER);
		splitPane.setRightComponent(plotAndControlsPanel);
		
		freezePlotButton = new JButton("");
		TextIcon t1 = new TextIcon(freezePlotButton, "Зафиксировать", TextIcon.Layout.VERTICAL);
		//RotatedIcon r1 = new RotatedIcon(t1, RotatedIcon.Rotate.DOWN);
		freezePlotButton.setIcon(t1);
		freezePlotButton.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e) {
				spectrometerPlotPanel.freezePlot();
				
			}
		});
		
		plotAndControlsPanel.add(freezePlotButton, BorderLayout.EAST);
		
		rootPanel.add(splitPane);
		mainFrame.setContentPane(rootPanel);
	}
	
	
	public void setSpectroPicture(BufferedImage picture) {
		spectrometerPicturePanel.setSpectroPicture(picture);
		spectrometerPicturePanel.repaint();
	}
	
	
	public void setScanRegion(int xleft, int ytop, int width, int height) {
		spectrometerPicturePanel.setScanRect(xleft, ytop, width, height);
	}
	
	
	public void setSpectroPlotData(List<XYDataItem> plotData) {
		XYSeries series = new XYSeries("Intensity");
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
        private SpectrometerSliderPanel spectrometerSliderPanel;
	private JPanel plotAndControlsPanel;
	private JButton freezePlotButton;
}

package ru.rsce.cansat.granum.spectrometer.client.gui;

import java.awt.BorderLayout;
import java.awt.LayoutManager;

import javax.swing.JPanel;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;

public class SpectrometerPlotPanel extends JPanel {

	
	private static final long serialVersionUID = 1061429927457876031L;

	
	public SpectrometerPlotPanel() {
		initialize();
	}

	public SpectrometerPlotPanel(LayoutManager layout) {
		super(layout);
		initialize();
	}

	public SpectrometerPlotPanel(boolean isDoubleBuffered) {
		super(isDoubleBuffered);
		initialize();
	}

	public SpectrometerPlotPanel(LayoutManager layout, boolean isDoubleBuffered) {
		super(layout, isDoubleBuffered);
		initialize();
	}
	
	
	public void setPlotData(XYSeries plotData) {
		chartDataset.removeAllSeries();
		chartDataset.addSeries(plotData);
		if (freezedDataset != null)
			chartDataset.addSeries(freezedDataset);
	}
	
	
	public void freezePlot() {
		try {
			freezedDataset = (XYSeries) chartDataset.getSeries(0).clone();
		} catch (CloneNotSupportedException e) {
			System.out.println(String.format("ТАКОГО НЕ МОЖЕТ БЫТЬ! %s", e.toString()));
		}
		freezedDataset.setKey("Сохраненная интенсивность");
	}

	
	private void initialize()	{
		chartDataset = new XYSeriesCollection();
		
		chart = ChartFactory.createXYLineChart(
				"Спектр", "номер колонки", "нм", chartDataset
		);
		
		chartPanel = new ChartPanel(chart);
		this.setLayout(new BorderLayout());
		this.add(chartPanel, BorderLayout.CENTER);
	}
	
	XYSeriesCollection chartDataset;
	XYSeries freezedDataset;
	JFreeChart chart;
	ChartPanel chartPanel;
}

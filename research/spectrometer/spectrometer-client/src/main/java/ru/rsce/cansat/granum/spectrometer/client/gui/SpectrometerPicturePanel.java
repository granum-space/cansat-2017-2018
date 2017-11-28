package ru.rsce.cansat.granum.spectrometer.client.gui;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.image.BufferedImage;

import javax.swing.JPanel;

public class SpectrometerPicturePanel extends JPanel {
	
	private static final long serialVersionUID = 1279611020549167575L;
	
	private class Rect {
		public Rect(int left_, int top_, int width_, int height_) {
			left = left_; top = top_; width = width_; height = height_;
		}
		
		public Rect scaled(float scaleX, float scaleY) {
			Rect retval = new Rect((int)(left*scaleX), (int)(top*scaleY), (int)(width*scaleX), (int)(height*scaleY));
			return retval;
		}
		
		public int left;
		public int top;
		public int width;
		public int height;
	}

	
	public void setSpectroPicture(BufferedImage image_) {
		image = image_;
	}
	
	
	public void setScanRect(int left, int top, int width, int height) {
		scanRect = new Rect(left, top, width, height);
	}
	
	
	@Override
	protected void paintComponent(Graphics g) {
		super.paintComponent(g);
		
		if (image == null)
			return;
				
		g.drawImage(image, 0, 0, this.getWidth(), this.getHeight(), this);
		g.setColor(Color.GREEN);
		
		if (scanRect != null)
		{
			float scaleX = (float)this.getWidth()/(float)image.getWidth();
			float scaleY = (float)this.getHeight()/(float)image.getHeight();
			Rect rect = scanRect.scaled(scaleX, scaleY);
			g.drawRect(rect.left, rect.top, rect.width, rect.height);
		}
	}

	
	Rect scanRect = null;
	private BufferedImage image = null;
}

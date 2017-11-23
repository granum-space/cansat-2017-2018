package ru.rsce.cansat.granum.spectrometer.client.gui;

import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.JPanel;

public class PicturePanel extends JPanel {

	/**
	 * Create the panel.
	 * @throws IOException 
	 */
	public PicturePanel() throws IOException {
		image = ImageIO.read(new File("/home/snork/Изображения/FEZ_035.png"));
	}

	
	void setImage(BufferedImage image_) {
		image = image_;
	}

	@Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        
        if (image != null)
    		g.drawImage(image, 0, 0, this);            
    }

	private BufferedImage image;
}

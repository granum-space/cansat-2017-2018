package ru.rsce.cansat.granum.spectrometer.client.gui;

import javax.swing.BoxLayout;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JSplitPane;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.transaction.xa.Xid;
import ru.rsce.cansat.granum.spectrometer.client.FrameMessageProcessor;
import ru.rsce.cansat.granum.spectrometer.client.Main;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Kirs
 */
public class SpectrometerSliderPanel extends JPanel{
    private int resY = 640, resX = 480;
    
    public void setResolution(int resX, int resY) {
        this.resX = resX;
        this.resY = resY;
        
        YStartSlider.setMaximum(resY);
        YStartSlider.repaint();
        
        YEndSlider.setMaximum(resY);
        YEndSlider.repaint();
        
        XCenterSlider.setMaximum(resX);
        XCenterSlider.repaint();
        
        XWidthSlider.setMaximum(Math.min(XCenterSlider.getValue(), resX - XCenterSlider.getValue()) * 2);
        XWidthSlider.repaint();
    }
    
    private JSlider YStartSlider, YEndSlider, XCenterSlider, XWidthSlider, MinResizeValSlider, MaxResizeValSlider;
    
    public SpectrometerSliderPanel() {
        this.setLayout(new BoxLayout(this, BoxLayout.X_AXIS));
        
        YStartSlider = new JSlider(JSlider.VERTICAL, 0, resY, 0);
        YStartSlider.setMajorTickSpacing(100);
        YStartSlider.setMinorTickSpacing(10);
        YStartSlider.setPaintTicks(true);
        YStartSlider.setPaintLabels(true);
        JSplitPane YStartGroup = new JSplitPane(JSplitPane.VERTICAL_SPLIT, YStartSlider, new JLabel("Y start"));
        YStartGroup.setDividerLocation(0.9);
        YStartGroup.setResizeWeight(1);
        this.add(YStartGroup);
        YStartSlider.addChangeListener(
            (ChangeEvent e) -> {
                if(YEndSlider.getValue() <= YStartSlider.getValue()) YEndSlider.setValue(YStartSlider.getValue() + 1);
                
                updateSliders();
            }
        );
        YStartSlider.setInverted(true);
        
        
        YEndSlider = new JSlider(JSlider.VERTICAL, 0, resY, resY);
        YEndSlider.setMajorTickSpacing(100);
        YEndSlider.setMinorTickSpacing(10);
        YEndSlider.setPaintTicks(true);
        YEndSlider.setPaintLabels(true);
        JSplitPane YEndGroup = new JSplitPane(JSplitPane.VERTICAL_SPLIT, YEndSlider, new JLabel("Y end"));
        YEndGroup.setDividerLocation(0.9);
        YEndGroup.setResizeWeight(1);
        this.add(YEndGroup);
        YEndSlider.addChangeListener(
            (ChangeEvent e) -> {
                if(YEndSlider.getValue() <= YStartSlider.getValue()) YStartSlider.setValue(YEndSlider.getValue() - 1);
                
                updateSliders();
            }
        );
        YEndSlider.setInverted(true);
        
        XCenterSlider = new JSlider(JSlider.VERTICAL, 0, resX, resX/2);
        XCenterSlider.setMajorTickSpacing(100);
        XCenterSlider.setMinorTickSpacing(10);
        XCenterSlider.setPaintTicks(true);
        XCenterSlider.setPaintLabels(true);
        JSplitPane XCenterGroup = new JSplitPane(JSplitPane.VERTICAL_SPLIT, XCenterSlider, new JLabel("X center"));
        XCenterGroup.setDividerLocation(0.9);
        XCenterGroup.setResizeWeight(1);
        this.add(XCenterGroup);
        XCenterSlider.addChangeListener(
            (ChangeEvent e) -> {
                XWidthSlider.setMaximum(Math.min(XCenterSlider.getValue(), resX - XCenterSlider.getValue()) * 2);
                XWidthSlider.repaint();
                
                updateSliders();
            }
        );
        XCenterSlider.setInverted(true);
        
        XWidthSlider = new JSlider(JSlider.VERTICAL, 0, resX/2, resX/2);
        XWidthSlider.setMajorTickSpacing(100);
        XWidthSlider.setMinorTickSpacing(10);
        XWidthSlider.setPaintTicks(true);
        XWidthSlider.setPaintLabels(true);
        JSplitPane XWidthGroup = new JSplitPane(JSplitPane.VERTICAL_SPLIT, XWidthSlider, new JLabel("X width"));
        XWidthGroup.setDividerLocation(0.9);
        XWidthGroup.setResizeWeight(1);
        this.add(XWidthGroup);
        XWidthSlider.addChangeListener(
            (ChangeEvent e) -> {
                updateSliders();
            }
        );
        XWidthSlider.setInverted(true);
        
        MinResizeValSlider = new JSlider(JSlider.VERTICAL, 0, 255, 0);
        MinResizeValSlider.setMajorTickSpacing(255);
        MinResizeValSlider.setMinorTickSpacing(5);
        MinResizeValSlider.setPaintTicks(true);
        MinResizeValSlider.setPaintLabels(true);
        JSplitPane MinResizeValGroup = new JSplitPane(JSplitPane.VERTICAL_SPLIT, MinResizeValSlider, new JLabel("Min resize val"));
        MinResizeValGroup.setDividerLocation(0.9);
        MinResizeValGroup.setResizeWeight(1);
        this.add(MinResizeValGroup);
        MinResizeValSlider.addChangeListener(
            (ChangeEvent e) -> {
                if(MinResizeValSlider.getValue() >= MaxResizeValSlider.getValue()) MaxResizeValSlider.setValue(MinResizeValSlider.getValue() + 1);
                updateSliders();
            }
        );
        
        MaxResizeValSlider = new JSlider(JSlider.VERTICAL, 0, 255, 255);
        MaxResizeValSlider.setMajorTickSpacing(255);
        MaxResizeValSlider.setMinorTickSpacing(5);
        MaxResizeValSlider.setPaintTicks(true);
        MaxResizeValSlider.setPaintLabels(true);
        JSplitPane MaxResizeValGroup = new JSplitPane(JSplitPane.VERTICAL_SPLIT, MaxResizeValSlider, new JLabel("Max resize val"));
        MaxResizeValGroup.setDividerLocation(0.9);
        MaxResizeValGroup.setResizeWeight(1);
        this.add(MaxResizeValGroup);
        MaxResizeValSlider.addChangeListener(
            (ChangeEvent e) -> {
                if(MinResizeValSlider.getValue() >= MaxResizeValSlider.getValue()) MinResizeValSlider.setValue(MaxResizeValSlider.getValue() - 1);
                updateSliders();
            }
        );
        
        updateSliders();
    }
    
    private void updateSliders() {
        Main.msgprocessor.setScanlineParams(XCenterSlider.getValue(), XWidthSlider.getValue(),
                                                    YStartSlider.getValue(), YEndSlider.getValue());
        Main.msgprocessor.setResampleParams(MinResizeValSlider.getValue(), MaxResizeValSlider.getValue());
    }
}
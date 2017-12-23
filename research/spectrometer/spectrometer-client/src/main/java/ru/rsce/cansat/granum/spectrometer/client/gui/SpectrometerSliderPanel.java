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
    private final int resY = 640, resX = 480;
    
    private JSlider YStartSlider, YEndSlider, XCenterSlider, XWidthSlider, MinResizeValSlider, MaxResizeValSlider;
    
    public SpectrometerSliderPanel() {
        this.setLayout(new BoxLayout(this, BoxLayout.X_AXIS));
        
        ChangeListener changeListener = new ChangeListener() {

            @Override
            public void stateChanged(ChangeEvent e) {
                FrameMessageProcessor.ScanlineParams params = Main.msgprocessor.getScanlineParams();
                params.yStart = YStartSlider.getValue();
                params.yStop = YEndSlider.getValue();
                params.xCenter = XCenterSlider.getValue();
                params.xWidth = XWidthSlider.getValue();
                Main.msgprocessor.setScanlineParams(params);
            }
        };
        
        YStartSlider = new JSlider(JSlider.VERTICAL, 0, resY, 0);
        JSplitPane YStartGroup = new JSplitPane(JSplitPane.VERTICAL_SPLIT, YStartSlider, new JLabel("Y start"));
        YStartGroup.setDividerLocation(0.9);
        YStartGroup.setResizeWeight(1);
        this.add(YStartGroup);
        YStartSlider.addChangeListener(changeListener);
        
        
        YEndSlider = new JSlider(JSlider.VERTICAL, 0, resY, resY);
        JSplitPane YEndGroup = new JSplitPane(JSplitPane.VERTICAL_SPLIT, YEndSlider, new JLabel("Y end"));
        YEndGroup.setDividerLocation(0.9);
        YEndGroup.setResizeWeight(1);
        this.add(YEndGroup);
        YEndSlider.addChangeListener(changeListener);
        
        XCenterSlider = new JSlider(JSlider.VERTICAL, 0, resX, resX/2);
        JSplitPane XCenterGroup = new JSplitPane(JSplitPane.VERTICAL_SPLIT, XCenterSlider, new JLabel("X center"));
        XCenterGroup.setDividerLocation(0.9);
        XCenterGroup.setResizeWeight(1);
        this.add(XCenterGroup);
        XCenterSlider.addChangeListener(changeListener);
        
        XWidthSlider = new JSlider(JSlider.VERTICAL, 0, resX/2, resX/2);
        JSplitPane XWidthGroup = new JSplitPane(JSplitPane.VERTICAL_SPLIT, XWidthSlider, new JLabel("X width"));
        XWidthGroup.setDividerLocation(0.9);
        XWidthGroup.setResizeWeight(1);
        this.add(XWidthGroup);
        XWidthSlider.addChangeListener(changeListener);
        
        MinResizeValSlider = new JSlider(JSlider.VERTICAL, 0, 255, 127);
        JSplitPane MinResizeValGroup = new JSplitPane(JSplitPane.VERTICAL_SPLIT, MinResizeValSlider, new JLabel("Min resize val"));
        MinResizeValGroup.setDividerLocation(0.9);
        MinResizeValGroup.setResizeWeight(1);
        this.add(MinResizeValGroup);
        
        MaxResizeValSlider = new JSlider(JSlider.VERTICAL, 0, 255, 127);
        JSplitPane MaxResizeValGroup = new JSplitPane(JSplitPane.VERTICAL_SPLIT, MaxResizeValSlider, new JLabel("Max resize val"));
        MaxResizeValGroup.setDividerLocation(0.9);
        MaxResizeValGroup.setResizeWeight(1);
        this.add(MaxResizeValGroup);
    }
}
import time
import struct
from itertools import chain
import csv
from enum import IntEnum
from math import acos, pi

import numpy as np
from stl import mesh
from pyquaternion import Quaternion

from pyqtgraph.Qt import QtGui, QtCore
from pyqtgraph import Qt
import pyqtgraph as pg
import pyqtgraph.opengl as gl

MESH_PATH = './cansat.stl'


class PlaneWidget(gl.GLViewWidget):
    def __init__(self, mesh_path, *args, **kwargs):
        super(PlaneWidget, self).__init__(*args, **kwargs)
        self.setCameraPosition(distance=40)

        g = gl.GLGridItem()
        g.scale(2,2,1)
        g.translate(0,0,-3)
        self.addItem(g)

        self.plane_axis = gl.GLAxisItem()
        self.plane_axis.setSize(x=500, y=500, z=500)
        self.addItem(self.plane_axis)

        verts = self._get_mesh_points(mesh_path)
        faces = np.array([(i, i+1, i+2,) for i in range(0, len(verts), 3)])
        colors = np.array([(0.0, 1.0, 0.0, 1.0,) for i in range(0, len(verts), 3)])
        self.mesh = gl.GLMeshItem(vertexes=verts, faces=faces, faceColors=colors, smooth=False, shader='shaded')
        self.addItem(self.mesh)
        self._update_mesh((0, 1,0,0,0))


    def on_new_records(self, records):
        quat = records[-1]
        self._update_mesh(quat)


    def _get_mesh_points(self, mesh_path):
        your_mesh = mesh.Mesh.from_file(mesh_path)
        points = your_mesh.points

        points = np.array(list(chain(*points)))
        i = 0
        nd_points = np.ndarray(shape=(len(points)//3, 3,) )
        for i in range(0, len(points)//3):
            nd_points[i] = points[i*3: (i+1)*3]

        return nd_points


    def _default_transform(self, target):
            target.scale(1/50, 1/50, 1/50)
            #target.rotate(90, 1, 0, 0)
            target.rotate(180, 0, 0, 1)


    def _update_mesh(self, quat):
        q = Quaternion(quat[1], quat[2], quat[3], quat[4])
        axis = q.get_axis(undefined=[1,0,0])
        angle = q.angle * 180.0/pi

        def do_things(target):
            target.resetTransform()
            self._default_transform(target)
            target.rotate(angle, axis[0], axis[1], axis[2])

        do_things(self.mesh)
        do_things(self.plane_axis)


class PlotWidget(pg.GraphicsLayoutWidget):
    pens = ['r', 'g', 'b', 'c']

    class CurveContainer:
        buffer_size = 10000
        def __init__(self, plot, pen, data_extractor):
            self.plot = plot
            self.pen = pen
            self.data_extractor = data_extractor

            self.curve = None
            self.x_data = []
            self.y_data = []

        def consume_records(self, records):
            self.x_data += [record[0] for record in records]
            self.y_data += [self.data_extractor(record) for record in records]

            if len(self.x_data) > self.buffer_size:
                self.x_data, self.y_data = self.x_data[-self.buffer_size: ], self.y_data[-self.buffer_size: ]

            if self.curve is None:
                self.curve = self.plot.plot(x=self.x_data, y=self.y_data, pen=self.pen)
            else:
                self.curve.setData(x=self.x_data, y=self.y_data)


    def __init__(self, *args, **kwargs):
        super(PlotWidget, self).__init__(*args, **kwargs)
        self.p1 = self.addPlot(row=0, col=0)

        self.curves = [
            self.CurveContainer(self.p1, self.pens[0], lambda record: record[1]),
            self.CurveContainer(self.p1, self.pens[1], lambda record: record[2]),
            self.CurveContainer(self.p1, self.pens[2], lambda record: record[3]),
            self.CurveContainer(self.p1, self.pens[3], lambda record: record[4]),
        ]


    def on_new_records(self, records):
        for curve in self.curves:
            curve.consume_records(records)



class MainWindow(QtGui.QDialog):
    def __init__(self):
        super(MainWindow, self).__init__()
        layout = QtGui.QVBoxLayout(self)
        pal = QtGui.QPalette()
        pal.setColor(QtGui.QPalette.Background, QtGui.QColor(0));
        self.setAutoFillBackground(True);
        self.setPalette(pal);

        self.plot_widget = PlotWidget(parent=self)
        self.plane_widget = PlaneWidget(mesh_path=MESH_PATH, parent=self)
        layout.addWidget(self.plane_widget, stretch=12)
        layout.addWidget(self.plot_widget, stretch=6)

    @QtCore.pyqtSlot(list)
    def on_new_record(self, records):
        self.plane_widget.on_new_records(records)
        self.plot_widget.on_new_records(records)



class DataGenerator(QtCore.QThread):
    new_record = QtCore.pyqtSignal(list)

    def __init__(self, *args, **kwargs):
        super(DataGenerator, self).__init__(*args, **kwargs)

    def run(self):
        while True:
            try:
                with open("ATTITUDE_QUATERNION.csv", "r") as stream:
                    source = csv.DictReader(stream)
                    i = 0
                    for line in source:
                        quat = (i, float(line["q1"]), float(line["q2"]), float(line["q3"]), float(line["q4"]),)
                        print(quat)
                        self.new_record.emit([quat])
                        i += 0.1
                        time.sleep(0.1)

                print("port_opened")
                break
            except IOError as e:
                #raise
                pass


app = QtGui.QApplication([])
pg.setConfigOptions(antialias=True)

win = MainWindow()
#win.resize(1024, 768)
#win.show()
win.showFullScreen()

data_generator = DataGenerator()
data_generator.new_record.connect(win.on_new_record)
data_generator.start()

exit(app.exec_())


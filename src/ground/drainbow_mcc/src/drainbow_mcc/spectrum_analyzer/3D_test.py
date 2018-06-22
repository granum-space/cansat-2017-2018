# -*- coding: utf-8 -*-
"""
Demonstrate use of GLLinePlotItem to draw cross-sections of a surface.

"""

from pyqtgraph.Qt import QtCore, QtGui
import pyqtgraph.opengl as gl
import pyqtgraph as pg
import numpy as np
import math
import csv

gamma = 0.80
intensityMax = 255

def Adjust(color, factor):
	if color == 0.0:
		result = 0 
	else:
		result = int(intensityMax * math.pow(color*factor, gamma))
	return result

def WavelengthToRGB(Wavelength):
	factor = 0
	Red = 0
	Green = 0
	Blue = 0

	if Wavelength >= 380 and Wavelength < 439:
		Red = -(Wavelength - 440) / (440 - 380)
		Green = 0.0
		Blue = 1.0
	elif Wavelength >= 440 and Wavelength < 489:
		Red   = 0.0
		Green = (Wavelength - 440) / (490 - 440)
		Blue  = 1.0
	elif Wavelength >= 490 and Wavelength < 509:
		Red   = 0.0
		Green = 1.0
		Blue  = -(Wavelength - 510) / (510 - 490)
	elif Wavelength >= 510 and Wavelength < 579:
		Red   = (Wavelength - 510) / (580 - 510)
		Green = 1.0
		Blue  = 0.0
	elif Wavelength >= 580 and Wavelength < 644:
		Red   = 1.0
		Green = -(Wavelength - 645) / (645 - 580)
		Blue  = 0.0
	elif Wavelength >= 645 and Wavelength < 780:
		Red   = 1.0
		Green = 0.0
		Blue  = 0.0
	else:
		Red   = 0.0
		Green = 0.0
		Blue  = 0.0

	if Wavelength >= 380 and Wavelength <= 419:
		factor = 0.3 + 0.7*(Wavelength - 380) / (420 - 380)
	elif Wavelength >= 420 and Wavelength <= 700:  
		factor = 1.0
	elif Wavelength >= 701 and Wavelength <= 780: 
		factor = 0.3 + 0.7*(780 - Wavelength) / (780 - 700)
	else:
		factor = 0.0

	R = Adjust(Red,   factor)
	G = Adjust(Green, factor)
	B = Adjust(Blue,  factor)
	print(Red, Green, Blue)
	return Red, Green, Blue

app = QtGui.QApplication([])
w = gl.GLViewWidget()
w.opts['distance'] = 40
w.setWindowTitle('pyqtgraph example: GLLinePlotItem')
w.showMaximized()

gx = gl.GLGridItem()
gx.rotate(90, 0, 1, 0)
gx.translate(-10, 0, 0)
w.addItem(gx)
gy = gl.GLGridItem()
gy.rotate(90, 1, 0, 0)
gy.translate(0, -10, 0)
w.addItem(gy)
gz = gl.GLGridItem()
gz.translate(0, 0, -10)
w.addItem(gz)

file = csv.DictReader(open("mega.csv"))

def map(x, in_min, in_max, out_min, out_max):
	if x > in_max:
		return out_max
	elif x < in_min:
		return out_min
	else:
		out = (x - in_min)*(out_max - out_min)/(in_max - in_min) + out_min
		return out

def get_true_nm(x):
	return int(map(x, in_min=1, in_max=230, out_min=680, out_max=380))

def map_xyz(nm, time, intensity, number, range_min, range_max):
	i = 0
	max_val = max(intensity)
	min_val = min(intensity)
	while i < len(nm):
		intensity[i] = map(intensity[i], min_val, max_val, -10, 0)
		nm[i] = map(int(nm[i]), 1, len(nm), -10, 10)
		i += 1
	time = np.linspace(-10, 10, 43) # temporary!!!
	xi = np.array(nm)
	yi = np.array([time[number]]*len(nm))
	zi = np.array(intensity)
	return xi, yi, zi

for row in file:
	number = int(row["number"])
	if number != 10:
		continue
	time = int(row["time_boot_ms"])
	intensity = []
	intensity.append(row["data"])
	import ast
	intensity = intensity[0]
	intensity = ast.literal_eval(intensity)
	intensity = list(reversed(intensity))
	nm = []
	nm = [el for el in range(len(intensity))]
	# x - нанометры
	# y - время
	# z - интенсивности
	x, y, z = map_xyz(nm, time, intensity, number, -10, 10)
	pts = np.vstack([x,y,z]).transpose()

	r = []
	b = []
	g = []
	a = []
	i = len(x)-1
	while i >= 0:
		q = get_true_nm(i)
		q = WavelengthToRGB(q)
		r.append(q[0])
		b.append(q[1])
		g.append(q[2])
		a.append(1.0)
		i -= 1

	pts = np.vstack([x,y,z]).transpose()
	colors = np.vstack([r, g, b, a]).transpose()
	plt = gl.GLLinePlotItem(pos=pts, color=colors, width=(30)/10., antialias=True)
	w.addItem(plt)

## Start Qt event loop unless running in interactive mode.
if __name__ == '__main__':
	import sys
	if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
		QtGui.QApplication.instance().exec_()


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

def map(x, in_min, in_max, out_min, out_max):
	if x > in_max:
		return out_max
	elif x < in_min:
		return out_min
	else:
		out = (x - in_min)*(out_max - out_min)/(in_max - in_min) + out_min
		return out

def get_true_nm(row_from_cropped_picture, y_upleft):
	row_from_big_picture = y_upleft + row_from_cropped_picture
	x = row_from_big_picture
	a1 = float(4244879/62257189230)
	a2 = float(-17684921/364077130)
	a3 = float(298720569791/31128594615)
	a4 = float(1445407214/19394763)
	y = int(a1*pow(x,3) + a2*pow(x,2) + a3*x + a4)
	if y > 650:
		return 650
	elif y < 405:
		return 405
	else:
		return y

def get_intensity_from_file(directory):
	intensity = []
	file = csv.DictReader(open(directory))
	for row in file:
		intensity.append(int(row["intensity"]))
	return intensity


class RgbCalculator:

	LEN_MIN = 380
	LEN_MAX = 780
	LEN_STEP = 5

	def __init__(self):
		pass

	X = [ 0.000160, 0.000662, 0.002362, 0.007242, 0.019110, 0.043400, 0.084736, 0.140638, 0.204492, 0.264737,
		0.314679, 0.357719, 0.383734, 0.386726, 0.370702, 0.342957, 0.302273, 0.254085, 0.195618, 0.132349,
		0.080507, 0.041072, 0.016172, 0.005132, 0.003816, 0.015444, 0.037465, 0.071358, 0.117749, 0.172953,
		0.236491, 0.304213, 0.376772, 0.451584, 0.529826, 0.616053, 0.705224, 0.793832, 0.878655, 0.951162,
		1.014160, 1.074300, 1.118520, 1.134300, 1.123990, 1.089100, 1.030480, 0.950740, 0.856297, 0.754930,
		0.647467, 0.535110, 0.431567, 0.343690, 0.268329, 0.204300, 0.152568, 0.112210, 0.081261, 0.057930,
		0.040851, 0.028623, 0.019941, 0.013842, 0.009577, 0.006605, 0.004553, 0.003145, 0.002175, 0.001506,
		0.001045, 0.000727, 0.000508, 0.000356, 0.000251, 0.000178, 0.000126, 0.000090, 0.000065, 0.000046,
		0.000033 ]

	Y = [ 0.000017, 0.000072, 0.000253, 0.000769, 0.002004, 0.004509, 0.008756, 0.014456, 0.021391, 0.029497,
		0.038676, 0.049602, 0.062077, 0.074704, 0.089456, 0.106256, 0.128201, 0.152761, 0.185190, 0.219940,
		0.253589, 0.297665, 0.339133, 0.395379, 0.460777, 0.531360, 0.606741, 0.685660, 0.761757, 0.823330,
		0.875211, 0.923810, 0.961988, 0.982200, 0.991761, 0.999110, 0.997340, 0.982380, 0.955552, 0.915175,
		0.868934, 0.825623, 0.777405, 0.720353, 0.658341, 0.593878, 0.527963, 0.461834, 0.398057, 0.339554,
		0.283493, 0.228254, 0.179828, 0.140211, 0.107633, 0.081187, 0.060281, 0.044096, 0.031800, 0.022602,
		0.015905, 0.011130, 0.007749, 0.005375, 0.003718, 0.002565, 0.001768, 0.001222, 0.000846, 0.000586,
		0.000407, 0.000284, 0.000199, 0.000140, 0.000098, 0.000070, 0.000050, 0.000036, 0.000025, 0.000018,
		0.000013 ]

	Z = [ 0.000705, 0.002928, 0.010482, 0.032344, 0.086011, 0.197120, 0.389366, 0.656760, 0.972542, 1.282500,
		1.553480, 1.798500, 1.967280, 2.027300, 1.994800, 1.900700, 1.745370, 1.554900, 1.317560, 1.030200,
		0.772125, 0.570060, 0.415254, 0.302356, 0.218502, 0.159249, 0.112044, 0.082248, 0.060709, 0.043050,
		0.030451, 0.020584, 0.013676, 0.007918, 0.003988, 0.001091, 0.000000, 0.000000, 0.000000, 0.000000,
		0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		0.000000 ]

	MATRIX_SRGB_D65 = [ 3.2404542, -1.5371385, -0.4985314, -0.9692660, 1.8760108,  0.0415560, 0.0556434, -0.2040259,  1.0572252 ]

	def Interpolate(self, values, index, offset):
		if offset == 0:
			return values[index]

		x0 = index * self.LEN_STEP
		x1 = x0 + self.LEN_STEP
		y0 = values[index]
		y1 = values[1 + index]

		return y0 + offset * (y1 - y0) / (x1 - x0)

	def GammaCorrect_sRGB(self, c):
		if c <= 0.0031308:
			return 12.92 * c

		a = 0.055
		return (1 + a) * math.pow(c, 1 / 2.4) - a

	def Clip(self, c):
		if c < 0:
			return 0
		if c > 1:
			return 1
		return c

	def Calc(self, wavelength):
		if wavelength < self.LEN_MIN or wavelength > self.LEN_MAX:
			return 0, 0, 0

		wavelength -= self.LEN_MIN;
		index = int(math.floor(wavelength / self.LEN_STEP))
		offset = wavelength - self.LEN_STEP * index

		x = self.Interpolate(values=self.X, index=index, offset=offset)
		y = self.Interpolate(values=self.Y, index=index, offset=offset)
		z = self.Interpolate(values=self.Z, index=index, offset=offset)

		m = self.MATRIX_SRGB_D65

		r = m[0] * x + m[1] * y + m[2] * z
		g = m[3] * x + m[4] * y + m[5] * z
		b = m[6] * x + m[7] * y + m[8] * z

		r = self.Clip(self.GammaCorrect_sRGB(r))
		g = self.Clip(self.GammaCorrect_sRGB(g))
		b = self.Clip(self.GammaCorrect_sRGB(b))

		return 255*r, 255*g, 255*b

c = RgbCalculator()

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

def map_xyz(nm, time, intensity, number, range_min, range_max):
	i = 0
	max_val = max(intensity)
	min_val = min(intensity)
	while i < len(nm):
		intensity[i] = map(intensity[i], min_val, max_val, -10, 0) #!
		nm[i] = map(int(nm[i]), 1, len(nm), -10, 10)
		i += 1
	time = np.linspace(-10, 10, 43) # temporary!!!
	xi = np.array(nm)
	yi = np.array([time[number]]*len(nm))
	zi = np.array(intensity)
	return xi, yi, zi

for row in file:
	number = int(row["number"])
	if number == 6 or number == 37 or number == 31 or number == 30:
		continue
	if number == 20:
		time = int(row["time_boot_ms"])
		time = 0
		intensity = []
		intensity.append(row["data"])
		import ast
		intensity = intensity[0]
		intensity = ast.literal_eval(intensity)
		intensity = list(reversed(intensity)) # в последней версии строчки считаются правильно
		nm = []
		nm = [el for el in range(len(intensity))]
		# x - нанометры
		# y - время
		# z - интенсивности
		x, y, z = map_xyz(nm, time, intensity, number, -10, 10)
		pts = np.vstack([x,y,z]).transpose()

		colors = []
		for i in range(1, len(x)+1):
			row_from_big_picture = i + 123
			if row_from_big_picture >= 128 and row_from_big_picture <= 321:
				wavelength = get_true_nm(i, 123)
			elif  row_from_big_picture < 128:
				wavelength = 650
			elif row_from_big_picture > 321:
				wavelength = 405
			r, g, b = c.Calc(wavelength)
			r = map(r, 0, 255, 0, 1)
			g = map(g, 0, 255, 0, 1)
			b = map(b, 0, 255, 0, 1)
			a = 1.0
			colors.append((r,g,b,a,))
			i += 1

		colors = np.array(colors)
		plt = gl.GLLinePlotItem(pos=pts, color=colors, width=(30)/10., antialias=True)
		w.addItem(plt)
'''
time = 0
intensity = get_intensity_from_file("650.csv")
nm = []
nm = [el for el in range(len(intensity))]
# x - нанометры
# y - время
# z - интенсивности
x, y, z = map_xyz(nm, time, intensity, 0, -10, 10)
pts = np.vstack([x,y,z]).transpose()

colors = []

for i in range(1, 480):
	if i >= 128 and i <= 321:
		wavelength = get_true_nm(i, 0)
	elif  i < 128:
		wavelength = 650
	elif i > 321:
		wavelength = 405
	print(wavelength)
	r, g, b = c.Calc(wavelength)
	r = map(r, 0, 255, 0, 1)
	g = map(g, 0, 255, 0, 1)
	b = map(b, 0, 255, 0, 1)
	a = 1.0
	colors.append((r,g,b,a,))
	i += 1

pts = np.vstack([x,y,z]).transpose()
colors = np.array(colors)
plt = gl.GLLinePlotItem(pos=pts, color=colors, width=(30)/10., antialias=True)
w.addItem(plt)'''

## Start Qt event loop unless running in interactive mode.
if __name__ == '__main__':
	import sys
	if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
		QtGui.QApplication.instance().exec_()


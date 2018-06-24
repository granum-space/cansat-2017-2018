from pyqtgraph.Qt import QtCore, QtGui
import pyqtgraph.opengl as gl
import pyqtgraph as pg
import numpy as np
import math
import csv
import ast

from RgbCalculator import RgbCalculator

ROW_MIN = 128
ROW_MAX = 321
WAVELENGTH_MIN = 405
WAVELENGTH_MAX = 650
MEGA_DIR = "mega.csv"
c = RgbCalculator()

def map(x, in_min, in_max, out_min, out_max):
	if x > in_max:
		return out_max
	elif x < in_min:
		return out_min
	else:
		out = (x - in_min)*(out_max - out_min)/(in_max - in_min) + out_min
		return out

"""
Считаем максимальные значения интенсивностей, 
чтобы в последствие правильно отобразить интенсивности на графике
"""
all_time = []
all_intensity = []
file = csv.DictReader(open(MEGA_DIR))
for row in file:
	all_time.append(int(row["time_boot_ms"]))
	intensity_row = []
	intensity_row.append(row["data"])
	intensity_row = intensity_row[0]
	intensity_row = ast.literal_eval(intensity_row)
	intensity_row = list(reversed(intensity_row)) # в последней версии строчки считаются правильно
	for el in intensity_row:
		all_intensity.append(int(el))
intensity_max = max(all_intensity)
time_max = max(all_time)
intensity_min = min(all_intensity)
time_min = min(all_time)

"""
Считаем нанометры, исходя из текущей строчки.
y_upleft - номер строчки оригинальной картинки, с которой начали обрезать ее для передачи
"""
def get_true_nm(row_from_cropped_picture, y_upleft):
	row_from_big_picture = y_upleft + row_from_cropped_picture
	x = row_from_big_picture
	a1 = float(4244879/62257189230)
	a2 = float(-17684921/364077130)
	a3 = float(298720569791/31128594615)
	a4 = float(1445407214/19394763)
	y = int(a1*pow(x,3) + a2*pow(x,2) + a3*x + a4)
	if y > WAVELENGTH_MAX:
		return WAVELENGTH_MAX
	elif y < WAVELENGTH_MIN:
		return WAVELENGTH_MIN
	else:
		return y

"""
Штучки pyqtgraph для постройки графика, оставляем как было
"""
app = QtGui.QApplication([])
w = gl.GLViewWidget()
w.opts['distance'] = 40
w.setWindowTitle('pyqtgraph example: GLLinePlotItem')
w.showMaximized()

"""
Строим координатные плоскости
"""
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

"""
Из существующих массивов нанометров, интенсивностей и времени получаем данные,
которые примет pyqtgraph для постройки одного графика
"""
def map_xyz(nm, time, intensity, number):
	for i in range(0, len(nm)):
		intensity[i] = map(int(intensity[i]), intensity_min, intensity_max, -10, 10)
		nm[i] = map(int(nm[i]), WAVELENGTH_MIN, WAVELENGTH_MAX, -10, 10)
	time = map(time, time_min, time_max, -10, 10)
	xi = np.array(nm)
	yi = np.array([time]*len(nm))
	zi = np.array(intensity)
	return xi, yi, zi

"""
Снова открываем мега-цсвшку и строим много графиков
"""
file = csv.DictReader(open(MEGA_DIR))
for row in file:
	number = int(row["number"])
	time = int(row["time_boot_ms"])
	y_upleft = int(row["y_upleft_crop"])
	intensity = []
	intensity.append(row["data"])
	# переводим intensity в нормальный список
	intensity = intensity[0]
	intensity = ast.literal_eval(intensity)
	intensity = list(reversed(intensity)) # в последней версии строчки считаются правильно
	nm = []
	colors = []
	# тут каждую точку графика окрашиваем в цвет, соответствующей ее длине волны
	for i in range(1, len(intensity)+1):
		row_from_big_picture = i + y_upleft
		if row_from_big_picture >= ROW_MIN and row_from_big_picture <= ROW_MAX:
			wavelength = get_true_nm(i, y_upleft)
		elif  row_from_big_picture < ROW_MIN:
			wavelength = WAVELENGTH_MAX
		elif row_from_big_picture > ROW_MAX:
			wavelength = WAVELENGTH_MIN
		r, g, b = c.Calc(wavelength)
		# pyqtgraph принимает значения компонент цветов от 0 до 1
		r = map(r, 0, 255, 0, 1)
		g = map(g, 0, 255, 0, 1)
		b = map(b, 0, 255, 0, 1)
		a = 1.0
		colors.append((r,g,b,a,))
		nm.append(wavelength)
		i += 1

	# ось x - нанометры
	# ось y - время
	# ось z - интенсивности
	x, y, z = map_xyz(nm, time, intensity, number)
	pts = np.vstack([x,y,z]).transpose()

	colors = np.array(colors)
	plt = gl.GLLinePlotItem(pos=pts, color=colors, width=(30)/10., antialias=True)
	w.addItem(plt)

## Start Qt event loop unless running in interactive mode.
if __name__ == '__main__':
	import sys
	if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
		QtGui.QApplication.instance().exec_()


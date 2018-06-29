from pyqtgraph.Qt import QtCore, QtGui
import pyqtgraph.opengl as gl
import pyqtgraph as pg
import numpy as np
import cv2
import math
import csv
import ast
import glob

from RgbCalculator import RgbCalculator

"""
ось x - нанометры
ось y - время
ось z - интенсивности
"""

ROW_MIN = 145
ROW_MAX = 385
WAVELENGTH_MIN = 405
WAVELENGTH_MAX = 650
up = 105
down = 135
x_min = 10 ## возможно понадобится поменять местами
x_max = -10 
y_min = -10
y_max = 10
z_min = -10
z_max = 10
y_upleft = None
nm = None
colors = None
reflection_max = None
reflection_min = None
time_max = None
time_min = None
state = None
prev_state = None
servo = None
intensity = None
prev_intensity = None
colors = None
MEGA_DIR = "mega.csv"
c = RgbCalculator()

def map(x, in_min, in_max, out_min, out_max):
	if x > in_max:
		return out_max
	elif x < in_min:
		return out_min
	else:
		if in_max - in_min == 0:
			out = (x - in_min)*(out_max - out_min) + out_min
		else:
			out = (x - in_min)*(out_max - out_min)/(in_max - in_min) + out_min
		return out

"""
Считаем максимальные значения интенсивностей, 
чтобы в последствие правильно отобразить интенсивности на графике
"""
def get_min_max(input_dir):
	all_time = []
	all_intensity = []
	file = csv.DictReader(open(input_dir))
	for row in file:
		all_time.append(int(row["time_boot_ms"]))
		intensity_row = []
		intensity_row.append(row["data"])
		intensity_row = intensity_row[0]
		intensity_row = ast.literal_eval(intensity_row)
		for el in intensity_row:
			all_intensity.append(int(el))
	intensity_max = max(all_intensity)
	time_max = max(all_time)
	intensity_min = min(all_intensity)
	time_min = min(all_time)
	return time_min, time_max, intensity_min, intensity_max

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
def map_xyz(x, y, z):
	for i in range(0, len(nm)): # рисуем точки, количество которых равно len(nm)
		x[i] = map(int(x[i]), WAVELENGTH_MIN, WAVELENGTH_MAX, x_min, x_max)
		z[i] = map(int(z[i]), reflection_min, reflection_max, z_min, z_max)
		y[i] = map(int(y[i]), time_min, time_max, y_min, y_max)
	xi = np.array(x)
	yi = np.array(y)
	zi = np.array(z)
	return xi, yi, zi

def get_nm_and_colors(intensity):
	nm = []
	colors = []
	for i in range(1, len(intensity)+1): # проходимся по всем строчкам
		row_from_big_picture = i + y_upleft
		if row_from_big_picture >= ROW_MIN and row_from_big_picture <= ROW_MAX:
			wavelength = get_true_nm(i, y_upleft)
		elif  row_from_big_picture < ROW_MIN:
			wavelength = WAVELENGTH_MAX
		elif row_from_big_picture > ROW_MAX:
			wavelength = WAVELENGTH_MIN
		r, g, b = c.Calc(wavelength)
		r = map(r, 0, 255, 0, 1) # pyqtgraph принимает значения компонент цветов от 0 до 1
		g = map(g, 0, 255, 0, 1)
		b = map(b, 0, 255, 0, 1)
		a = 1.0
		nm.append(wavelength)
		colors.append((r,g,b,a,))
	colors = np.array(colors)
	return nm, colors
"""
Снова открываем мега-цсвшку и строим много графиков
"""
xs = []
ys = []
zs = []
file = csv.DictReader(open(MEGA_DIR))
for row in file:
	if servo == up:
		state = "UP"
	elif servo == down:
		state = "DOWN"

	number = int(row["number"])
	time = int(row["time_boot_ms"])
	servo = int(row["servo"])
	y_upleft = int(row["y_upleft_crop"])
	#y_upleft = 123
	intensity = []
	intensity.append(row["data"])
	""" переводим intensity в нормальный список """
	intensity = intensity[0]
	intensity = ast.literal_eval(intensity)
	nm = []
	colors = []
	""" окрашиваем каждую точку графика в цвет, соответствующей ее длине волны """
	nm, colors = get_nm_and_colors(intensity)
	time = ([time]*len(nm))

	if state == "UP" and prev_state == "DOWN":
		xs.append(nm)
		ys.append(time)
		reflection = []
		for i in range(len(intensity)):
			reflection.append(math.log(intensity[i]/prev_intensity[i])*100)
		zs.append(reflection)

	prev_state = state
	prev_intensity = intensity

all_zs = []
all_time = []
for i in range(len(zs)):
	for j in range(len(zs[i])):
		all_zs.append(zs[i][j])
		all_time.append(ys[i][j])

reflection_min = min(all_zs)
reflection_max = max(all_zs)
time_min = min(all_time)
time_max = max(all_time)

for i in range(len(zs)):
	x, y, z = map_xyz(x=xs[i], y=ys[i], z=zs[i])
	x = x[:190] ## потом убрать
	y = y[:190]
	z = z[:190]
	print(reflection_max, reflection_min)
	pts = np.vstack([x,y,z]).transpose()
	plt = gl.GLLinePlotItem(pos=pts, color=colors, width=(30)/10., antialias=True)
	w.addItem(plt)

## Start Qt event loop unless running in interactive mode.
if __name__ == '__main__':
	import sys
	if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
		QtGui.QApplication.instance().exec_()


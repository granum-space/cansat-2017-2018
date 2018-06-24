from math import pow
import csv

OUTPUT_DIR = "nm_from_row.csv"

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

ostream = open(OUTPUT_DIR, mode="w", newline="")
fieldnames = ["nm", "row"]
writer = csv.DictWriter(ostream, fieldnames)
writer.writeheader()

ROW_MIN = 128
ROW_MAX = 321

for x in range(ROW_MIN, ROW_MAX+1):
	y = get_true_nm(x, 0)
	line = x
	nm = y
	row = {"nm": nm, "row":line}
	writer.writerow(row)
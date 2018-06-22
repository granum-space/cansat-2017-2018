import cv2
import csv

def get_spectrum_from_img(img):
    intensity = []

    height = img.shape[0]
    width = img.shape[1]

    i = height-1
    while i >= 0:
        j = width-1
        summ = 0
        while j >= 0:
            summ += img[i, j]
            j -= 1
        intensity.append(summ)
        i -= 1

    return intensity

OUTPUT_DIR = "graph.csv"

img = cv2.imread("img.png")
width = 40
height = 250
x_upleft = 330
y_upleft = 140

y_min = int(y_upleft)
y_max = int(y_upleft + height)
x_min = int(x_upleft)
x_max = int(x_upleft + width)

#img = img[y_min:y_max, x_min:x_max]
img = cv2.cvtColor(img, cv2.COLOR_RGB2YUV)
y_component, u_component, v_component = cv2.split(img)
img = y_component

#cv2.imwrite("result.png", img)

array = get_spectrum_from_img(img)

with open(OUTPUT_DIR, mode="w", newline="") as ostream:
    fieldnames = ["nm", "intensity"]
    writer = csv.DictWriter(ostream, fieldnames)
    writer.writeheader()

    i = 0
    for el in array:
        nm = i
        intensity = el
        row = {"nm": nm, "intensity": intensity}

        writer.writerow(row)
        i += 1

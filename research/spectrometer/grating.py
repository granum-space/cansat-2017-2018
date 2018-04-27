#!python3

from math import asin, pi, sin, sqrt, fabs, isclose, tan

dist = 2 * 10**-3 # m
t = 1000 # lines per mm
k = +1

print("Период решеки: %d штрихов на мм" % (t,))
print("номер спектра: %d" % (k,))

alpha_mid_def = 50 *pi/180 # rad

la1 = 400 * 10**-9 #nm
la2 = 720 * 10**-9 #nm


d = 10**-3/t
la_mid = (la1+la2)/2
print("Заданный угол центра спектра (%d нм): %f град" % (la_mid*10**9, alpha_mid_def*180/pi))
print()


def thetta_from_alpha(alpha, wavelength, d, k):
    sin_thetta = sin(alpha) - k*wavelength/d
    return asin(sin_thetta)

def alpha_from_thetta(thetta, wavelength, d, k):
    sin_alpha = k*wavelength/d + sin(thetta)
    return asin(sin_alpha)



thetta = thetta_from_alpha(alpha_mid_def, la_mid, d, k)
print( "угол отклонения решетки %f град" % (thetta*180/pi,) )

alpha1, alpha_mid, alpha2 = [alpha_from_thetta(thetta, x, d, k) for x in (la1, la_mid, la2, )]

print("углы отклонения от нормали решетки для %d %d %d нм == %f %f %f град" % (
    la1*10**9, la_mid*10**9, la2*10**9,
    alpha1*180/pi, alpha_mid*180/pi, alpha2*180/pi, 
))

delta_alpha = alpha2-alpha1
print("угловая ширина спектра: %f" % (delta_alpha*180/pi,))

half_delta_alpha = delta_alpha/2
swath = tan(half_delta_alpha)*dist * 2
print("Линейная ширина спектра: %f мм на расстоянии %f мм от решетки" % (swath*10**3, dist*10**3, ))






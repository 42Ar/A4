import math

U = math.sqrt(2)*230
w = 2*math.pi*50
C = 15e-9*1.2
R = 10e3

I = U/(R - 1j/(w*C))

# P = avg(re(Iexp(itau))re(Uexp(itau)))
#   = avg(IUexp(2itau) + I*U*exp(-2itau))/4 + (I*U + U*I)/2
#   = re(I*U)/2

# U_r = U exp(-t/(CR))
# P = U_r**2/R

print("W", U**2*C/2)
print("P", (I.conjugate()*U).real/2)
print("I_surge", U/R)
print("P_surge", U**2/R)
print("U_R", abs(I*R))
print("tau", R*C)
print("I", abs(I))
print("fuse I", abs(I)/.75)

print("short power", U*U/(2*R))

# is voltage across cap dangerous ?
# use XY filter caps and fuseable resistor


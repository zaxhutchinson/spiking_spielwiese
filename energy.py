import matplotlib.pyplot as plt
import scipy.signal as spsig

energy=[]

with open("energy.out") as f:
    for line in f:
        energy.append(float(line.strip()))


sg_energy = spsig.savgol_filter(energy,10001,3)

plt.plot(energy)
plt.plot(sg_energy)
plt.show()
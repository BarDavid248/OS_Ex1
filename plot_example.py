# OS 24 EX1
import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv(r"./yoav_data.csv")
data = data.to_numpy()

KiB = 1024
MiB = 1024**2

l1_size = 192 * KiB
l2_size = 5 * MiB
l3_size = 48 * MiB

plt.plot(data[:, 0], data[:, 1], label="Random access")
plt.plot(data[:, 0], data[:, 2], label="Sequential access")
plt.xscale('log')
plt.axvline(x=l1_size, label=f"L1 ({l1_size//KiB} KiB)", c='r')
plt.axvline(x=l2_size, label=f"L2 ({l2_size//MiB} MiB)", c='g')
plt.axvline(x=l3_size, label=f"L3 ({l3_size//MiB} MiB)", c='brown')
plt.legend()
plt.title("Latency as a function of array size")
plt.ylabel("Latency (ns)")
plt.xlabel("Bytes allocated (log scale)")
plt.savefig('plot.png')

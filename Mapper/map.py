#!/usr/bin/python3

import matplotlib.pyplot as plt
import subprocess

#rc = subprocess.call("~/Dropbox/school/soen422/project/retrieve_trajectory.sh", shell=True)

x = open("x.txt", "r")
y = open("y.txt", "r")

x_array = x.read().split(',')
y_array = y.read().split(',')

x_array.pop()
y_array.pop()

x_array = [int(i) for i in x_array]
y_array = [int(i) for i in y_array]

plt.plot(x_array, y_array, 'x--', lw=2, color='black', ms=10)
plt.show()

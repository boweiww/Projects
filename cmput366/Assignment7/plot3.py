import os
from matplotlib import pyplot as plt
import numpy as np
import matplotlib 

import importlib
#importlib.import_module('mpl_toolkits').__path__
from mpl_toolkits.mplot3d.axes3d import Axes3D
# from mpl_toolkits.mplot3d import axes3d
    # Copyright (c) HAOTIAN ZHU ,COMPUT366,University Of Alberta All Rights Reserved.
    # You May Use, Distribute Or Modify This Code Under Term And 
    # Condition Of Code Of Students Behavior At University Of Alberta.


    # Author: Haotian Zhu
    # If You Have Any Question Please contact haotian1@ualberta.ca.

print(matplotlib.__version__)

fig = plt.figure()
ax = fig.add_subplot(1,1,1, projection='3d')


filename = 'value.npy'
x=[]
y=[]
for i in range(50):
    x.append(-1.2+ (1.7/50)*i)
    y.append(-0.07+(0.14/50)*i)

if os.path.exists(filename):

    data = np.load(filename)

    for n in range(50):
        nx = [-1.2+ (1.7/50)*n]*50
        ax.plot_wireframe(nx,y,data[n])
    for n in range(50):
        ny = [-0.07+(0.14/50)*n]*50
        z = data[:][n]
        ax.plot_wireframe(x,ny,z)


    plt.ylim([-0.07,0.07])
    plt.xlim([-1.2,0.6])
    plt.xlabel('position')
    plt.ylabel('v')
    plt.legend()
    plt.show()
import random
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from PIL import Image
import time

def readMap():
    size = 100
    map = []
    f = open("map1.txt")

    for line in f:
        line_l = line.split()
        

        if len(line_l) == size:
            line_li = [eval(i) for i in line_l]
            newline_li = []
            for num in line_li:
                if num == 0:
                    newline_li.append(100)
                else:
                    newline_li.append(0)
            line_li = [x * 50 for x in line_li]
            map.append(newline_li)
    map_np = np.asarray(map)
    fr = open("robotPos.txt")
    for line in fr:
        line_l = line.split()
        map_np[int(line_l[1]), int(line_l[0])] = 10

    fr = open("currPos.txt")
    for line in fr:
        line_l = line.split()
        map_np[int(line_l[1]), int(line_l[0])] = 52

    fr = open("goalPos.txt")
    for line in fr:
        line_l = line.split()
        map_np[int(line_l[1]), int(line_l[0])] = 90
    return map_np

def readPlan(map):
    f = open("output.txt") 
    fig, ax = plt.subplots()
    img, = ax.plot(map)
    for line in f:
        pairs = line.split()
        for i in range(len(pairs)):
            pair = pairs[i]
            pairl= pair.split(",")
            map[int(pairl[1]), int(pairl[0])] = i + 6
        img.set_data(map)
        fig.canvas.draw()
        fig.canvas.flush_events()
        time.sleep(0.1)
    return map

def readPlan2(map):#robot color
    f = open("output.txt")
    
    mapframs = []

    lastpairs = []
    for line in f:
        pairs = line.split()
        
        for i in range(len(pairs)):
            pair = pairs[i]
            
            pairl= pair.split(",")
            
            if len(lastpairs) != 0:
                lastpair = lastpairs[i]
                lastpairl = lastpair.split(",")
                map[int(lastpairl[1]), int(lastpairl[0])] = 99
                map[int(pairl[1]), int(pairl[0])] = i*8+12

        mapframs.append(np.copy(map))
        lastpairs = pairs
    return mapframs




map = readMap()
mapframes = readPlan2(map)

mpf = mapframes[0]
plt.imshow(mpf, cmap = mpl.colormaps['nipy_spectral'])
plt.show()
def update(i):
    mpf = mapframes[i]
    matrice.set_array(mpf)

fig, ax = plt.subplots()
matrice = ax.matshow(mpf, cmap = mpl.colormaps['nipy_spectral'])
ani = animation.FuncAnimation(fig, update, frames = len(mapframes), interval = 1)
ani.save("ani.gif")
plt.show()

# for i in range(obstacles):
#     random_x = random.randint(0, size - 1) 
#     random_y = random.randint(0, size - 1) 
#     map[random_y][random_x] = 1



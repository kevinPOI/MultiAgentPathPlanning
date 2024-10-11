import random
import numpy as np
import matplotlib.pyplot as plt

def add_rec(map, x0, y0, x1, y1):
    map[y0:y1, x0:x1] = 1

size = 100
obstacles = 200
map = np.zeros((size, size), dtype=int)
add_rec(map, 10, 10, 90, 20)
add_rec(map, 10, 25, 25, 70)
add_rec(map, 30, 30, 45, 85)
add_rec(map, 50, 25, 65, 70)
add_rec(map, 70, 30, 85, 85)
add_rec(map, 50,84, 90, 99)
add_rec(map, 5,80, 30, 95)

# for i in range(obstacles):
#     random_x = random.randint(0, size - 1) 
#     random_y = random.randint(0, size - 1) 
#     map[random_y][random_x] = 1
with open("map1.txt", "w") as f:
    f.write("height " + str(size) + "\n")
    f.write("width " + str(size) + "\n")
    np.savetxt(f, map.astype(int), fmt='%i', delimiter=" ")

plt.imshow(map)
plt.show()
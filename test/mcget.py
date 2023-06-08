import json
from time import sleep
import memcache

mc = memcache.Client(['127.0.0.1:11211'], debug=True)
prev_time = 0
while True:
    x = mc.get('x')
    y = mc.get('y')
    z = mc.get('z')
    yaw = mc.get('yaw')
    pitch = mc.get('pitch')
    roll = mc.get('roll')
    time = mc.get('time')
    aruco = mc.get('aruco')
    x_coor = mc.get('x_coor')
    y_coor = mc.get('y_coor')
    find_circle = mc.get('find_circle')
    circle = mc.get('circle')
    print(circle)
    sleep(0.5)
#!/usr/bin/env python

'''
test mavlink messages
'''

import sys, struct, time, os
import rospy
import matplotlib.pyplot as plt
from pylab import plot, subplot, axis, grid, title, xlabel, ylabel, ylim, draw, ion, legend
from curses import ascii
from pymavlink import mavutil, mavparm
import numpy as np
import time
	
def wait_gps_position(value1, value2, value3):
	setpoint_list.append (value1)
	del setpoint_list[0]

	error_list.append(value2)
	del error_list[0]


	current_heigth_list.append (value3)
	del current_heigth_list[0]


	current_heigth_plt.set_ydata(current_heigth_list)
	setpoint_plt.set_ydata(setpoint_list)
	error_plt.set_ydata(error_list)

	draw() 


setpoint_list = [0]*500
error_list = [0]*500
current_heigth_list = [0]*500

ion()
subplot (111)

setpoint_plt, = plot(setpoint_list, 'red', label="Setpoint (locked heigth)", linewidth=4)
error_plt, = plot(error_list, 'blue', label="Error (locked heigth-current heigth)", linewidth=4)
current_heigth_plt, = plot(current_heigth_list, 'black', label="Current heigth", linewidth=4)

legend(loc=2)

ylim([0,1000])
ylabel('Meters')
grid (True)
title ("plotting altitude")
draw() # redraw plot


# wait for the heartbeat msg to find the system ID
error=0
setpoint=1
current = 5

while not rospy.is_shutdown():
	wait_gps_position(error, setpoint, current)
	error+=1
	setpoint+=2
	current +=20
	time.sleep(0.1)




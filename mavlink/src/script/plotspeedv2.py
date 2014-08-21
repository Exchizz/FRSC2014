#!/usr/bin/env python

import sys, struct, time, os
import rospy
import matplotlib.pyplot as plt
from pylab import plot, subplot, axis, grid, title, xlabel, ylabel, ylim, draw, ion, legend, autoscale
from curses import ascii
from pymavlink import mavutil, mavparm
import numpy as np
from std_msgs.msg import Float32MultiArray


setpoint = 0.0
error = 0.0
current = 0.0
def plotnow():
	global setpoint, error, current


	print "setpoint :" + str(setpoint) + " error: " + str(error) + " current:" + str(current)
        setpoint_list.append (setpoint)
        del setpoint_list[0]

        error_list.append(error)
        del error_list[0]

        current_list.append (current)
        del current_list[0]

        current_height_plt.set_ydata(current_list)
        setpoint_plt.set_ydata(setpoint_list)
        error_plt.set_ydata(error_list)

        draw()


	

def callback(data):
	global setpoint, error, current
	setpoint = data.data[0]
	error = data.data[1]
	current = data.data[2]


setpoint_list = [0]*500
error_list = [0]*500
current_list = [0]*500

ion()
subplot (211)
setpoint_plt, = plot(setpoint_list, 'red', label="Setpoint", linewidth=4)
current_height_plt, = plot(current_list, 'black', label="Current", linewidth=4)
grid (True)
legend(loc=2)
ylabel('Error')
title ("Plotting heigths")
ylim([0,500])


#autoscale(True, True, True)

subplot(212)
error_plt, = plot(error_list, 'blue', label="Error", linewidth=4)
#autoscale(True, True, True)
grid (True)
legend(loc=2)


ylim([-100,100])
ylabel('Error')
title ("Plotting error")
draw() # redraw plot


# run simultaenously.
rospy.init_node('plotter', anonymous=True)

rospy.Subscriber("/fmInformation/error", Float32MultiArray, callback)


#rospy.spin()
while not rospy.is_shutdown():
	time.sleep(0.1)
	plotnow()

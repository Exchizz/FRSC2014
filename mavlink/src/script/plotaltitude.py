#!/usr/bin/env python

import sys, struct, time, os
import rospy
import matplotlib.pyplot as plt
from pylab import plot, subplot, axis, grid, title, xlabel, ylabel, ylim, draw, ion, legend
from curses import ascii
from pymavlink import mavutil, mavparm
import numpy as np
from std_msgs.msg import Float32MultiArray

	

from optparse import OptionParser
parser = OptionParser("mavtester.py [options]")

parser.add_option("--baudrate", dest="baudrate", type='int',
                  help="master port baud rate", default=57600)
parser.add_option("--device", dest="device", default=None, help="serial device")
parser.add_option("--source-system", dest='SOURCE_SYSTEM', type='int',
                  default=255, help='MAVLink source system for this GCS')
(opts, args) = parser.parse_args()

if opts.device is None:
    print("You must specify a serial device")
    sys.exit(1)


setpoint = 0.0
error = 0.0
def callback(data):
	global setpoint, error
	setpoint = data.data[0]
	error = data.data[1]
#def wait_heartbeat(m):
#    '''wait for a heartbeat so we know the target system IDs'''
#    print("Waiting for APM heartbeat")
#    msg = m.recv_match(type='HEARTBEAT', blocking=True)
#    print("Heartbeat from APM (system %u, component %u, base_mode %d)" % (m.target_system, m.target_system, m.base_mode ))

count = 1
avg_heigth = 0
def wait_gps_position(m, error, setpoint):
	global count, avg_heigth
	msg = m.recv_match(type='GLOBAL_POSITION_INT', blocking=True)

        setpoint_list.append (setpoint)
        del setpoint_list[0]

        error_list.append(error)
        del error_list[0]

        current_height_list.append (msg.alt/1000.0)
        del current_height_list[0]

	avg_heigth += (msg.alt/1000.0)

	print "height: " + str(msg.alt/1000.0) + " avg: " + str(avg_heigth/count)
        current_height_plt.set_ydata(current_height_list)
        setpoint_plt.set_ydata(setpoint_list)
        error_plt.set_ydata(error_list)

	count+=1
        draw()
#	print "data"


setpoint_list = [0]*500
error_list = [0]*500
current_height_list = [0]*500

ion()
subplot (111)

setpoint_plt, = plot(setpoint_list, 'red', label="Setpoint (locked height)", linewidth=4)
error_plt, = plot(error_list, 'blue', label="Error (locked height-current height)", linewidth=4)
current_height_plt, = plot(current_height_list, 'black', label="Current height", linewidth=4)

legend(loc=2)

ylim([0,100])
ylabel('Meters')
grid (True)
title ("plotting altitude")
draw() # redraw plot


# create a mavlink serial instance
master = mavutil.mavlink_connection(opts.device, baud=opts.baudrate, source_system=opts.SOURCE_SYSTEM)

# run simultaenously.
rospy.init_node('plotter', anonymous=True)

rospy.Subscriber("/fmInformation/error", Float32MultiArray, callback)

# wait for the heartbeat msg to find the system ID
while not rospy.is_shutdown():
		wait_gps_position(master, error, setpoint)
		time.sleep(0.1)

print("Sending all message types")
#mavtest.generate_outputs(master.mav)



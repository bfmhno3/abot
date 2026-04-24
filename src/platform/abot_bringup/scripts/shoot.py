#!/usr/bin/env python

import rospy
import serial
import time
from std_msgs.msg import String


class AbotShoot():
    def __init__(self):
        # Give the node a name
        rospy.init_node('abot_shoot', anonymous=False)
        serial_port = rospy.get_param('~port', '/dev/shoot')
        baud_rate = rospy.get_param('~baud_rate', 9600)
        self.ser = serial.Serial(port=serial_port, baudrate=baud_rate, parity="N", bytesize=8, stopbits=1)

        # Subscribe to the /shoot topic
        rospy.Subscriber('/shoot', String, self.shoot_continue)

        rospy.loginfo("Shoot to ar_tag")

    def shoot_continue(self, msg):
        self.ser.write(b'\x55\x01\x12\x00\x00\x00\x01\x69')
        print(0)
        time.sleep(0.1)
        self.ser.write(b'\x55\x01\x11\x00\x00\x00\x01\x68')


if __name__ == '__main__':
    try:
        AbotShoot()
        rospy.spin()
    except:
        pass

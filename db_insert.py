#!/usr/bin/python
# This utility will be called from the kernel module interceptor.c.
# It has to read the data written in the character device and then parse it for tokens which
# can then be inserted into the DB.
# The schema for the DB is:
# FILE		TIMESTAMP		SID		ACTION
#
# Sample record written in the character device is:
# /home/nitin/myfile.txt|||<timestamp>|||1712380443|||READ
#  
##################################################################################################
import mysql.connector
import time
import datetime

t = time.time()

char_device_name = "/dev/my_char_dev"

fname = open(char_device_name , 'r')
record = fname.read()
record = record.split('$')

st = datetime.datetime.fromtimestamp(t).strftime('%Y-%m-%d %H:%M:%S')
 
query = (""" INSERT INTO SID_LOGS (FILENAME , SID , TIMESTAMP , ACTION) VALUES (%s , %s , %s , %s)""" , (record[0] , record[1] , st , record[2]))	


# DB code starts here
cnx = mysql.connector.connect(user='root', password='nitin',
                              host='localhost',
                              database='LOGS')
cur = cnx.cursor()

cur.execute(query)

cnx.close()

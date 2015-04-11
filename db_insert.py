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
log_file_name = "/home/nitin/thesis/modules/db_insert.log"

fname = open(char_device_name , 'r')
logfname = open(log_file_name , 'w')
record = fname.read()
record = record.split('$')
logfname.write("called for \n")
st = datetime.datetime.fromtimestamp(t).strftime('%Y-%m-%d %H:%M:%S')
#st = "\"" + st + "\""
 
#query = "INSERT INTO SID_LOGS (FILENAME , SID , TIMESTAMP , ACTION) VALUES ( %s , %s , %s , %s )" ,( str(record[0])  , str(record[1]) , st  , str(record[2]) )

#print query
# DB code starts here
cnx = mysql.connector.connect(user='root', password='nitin',
                              host='localhost',
                              database='LOGS')
cur = cnx.cursor()

cur.execute("""INSERT INTO SID_LOGS (FILENAME , SID , TIMESTAMP , ACTION) VALUES ( %s , %s , %s , %s )""" , ( str(record[0])  , str(record[1]) , st  , str(record[2]) ) )
cnx.commit()
cnx.close()

fname.close()
logfname.close()
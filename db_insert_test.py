#!/usr/bin/python
# can then be inserted into the DB.
# The schema for the DB is:
# FILE		TIMESTAMP		SID		ACTION
#
# Sample record written in the character device is:
# /home/nitin/myfile.txt|||<timestamp>|||1712380443|||READ
#  
##################################################################################################
import MySQLdb
import time
import datetime

t = time.time()


cnx = MySQLdb.connect(user='root', passwd='nitin',
                              host='localhost',
                              db='LOGS')
cur = cnx.cursor()

cur.execute("""INSERT INTO SID_LOGS (FILENAME , SID , TIMESTAMP , ACTION) VALUES (%s , %s , %s , %s )""", ( "file2" , "sidt" , "100" , "OPEN") )
cnx.commit()

cnx.close()


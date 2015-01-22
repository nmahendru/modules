#!/usr/bin/python
import subprocess
import re
kcmd = ['truncate' , '--size=1k' , '/var/log/kern.log']
scmd = ['truncate' , '--size=1k' , '/var/log/syslog']
subprocess.call(kcmd)
subprocess.call(scmd)
rcmd = ['sudo' , 'rm' , '/var/log/*.gz']
subprocess.call(rcmd)
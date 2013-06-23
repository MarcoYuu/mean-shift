#!/usr/bin/python
# coding: UTF-8
import sys
import os
import datetime

print "start"

for Hs in [8, 16, 32, 64]:
	for Hr in [8, 16, 32, 64]:
		for loop in [5, 10, 30]:
			command = "./build/mshift "+sys.argv[1]+" "+str(Hs)+" "+str(Hr)+" "+str(loop)+" 8"
			print command
			os.system(command);

dir_name =datetime.datetime.today().strftime("%Y-%m-%d_%H-%M-%S")
os.system("mkdir result/"+dir_name)
os.system("mv *.png ./result/"+dir_name)

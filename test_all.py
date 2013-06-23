#!/usr/bin/python
# coding: UTF-8
import os
import glob

print "start"

for img in glob.glob("./img/*"):
	command = "./test.py "+ img +" | tee -a log"
	print command
	os.system(command);

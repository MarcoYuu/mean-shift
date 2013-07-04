#!/usr/bin/python
# coding: UTF-8
import sys
import os

print "start"

loop = 10
for Hs in [16, 32]:
	for Hr in [32, 64]:
		command = "./build/mshift "+sys.argv[1]+" "+str(Hs)+" "+str(Hr)+" "+str(loop)+" 8 unitball"
		print command
		os.system(command);
		command = "./build/mshift "+sys.argv[1]+" "+str(Hs)+" "+str(Hr)+" "+str(loop)+" 8 fukunaga"
		print command
		os.system(command);
		command = "./build/mshift "+sys.argv[1]+" "+str(Hs)+" "+str(Hr)+" "+str(loop)+" 8 gaussian"
		print command
		os.system(command);

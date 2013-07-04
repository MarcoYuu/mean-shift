#!/usr/bin/python
# coding: UTF-8
import sys
import os
import glob

for name in glob.glob("*.png"):
	name_mod = name.replace("k0_th8", "unitball")
	name_mod = name_mod.replace("k1_th8", "fukunaga")
	name_mod = name_mod.replace("k2_th8", "gaussian")
	print name_mod
	os.system("mv "+name+" "+name_mod) 
	print name+" -> "+name_mod

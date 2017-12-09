#!/usr/bin/python

import os, sys

# output file
fileout = "./headers.h"

# Open a file
path = "./header/"
dirs = os.listdir( path )

# This would print all the files and directories
with open(fileout, 'w') as fout:
    for file in dirs:
        fileDir = path+file
        with open(fileDir, 'r') as f:
            line = f.readline()
            while(line):
                print(line)
                fout.write(line)
                line = f.readline()

        fout.write("\n//=================================================")
        fout.write("\n//=================================================")
        fout.write("\n")
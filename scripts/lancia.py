#!/usr/bin/env python
import os, re


crystalz_vals = [10., 15., 20.]

for crystalz in crystalz_vals:
    
    label = "crystal_z_%.1f" % (crystalz)
    command = "./sendJobsOnHercules.py -l %s -b /gwpool/users/benaglia/TIMING/G4_SingleBar/ -e build/SingleBar -o out_%s -O /gwteraz/users/benaglia/TIMING/G4_SingleBar/ -c config.cfg --crystalz %s -s " % (label,label,str(crystalz))
    print command
    os.system(command)

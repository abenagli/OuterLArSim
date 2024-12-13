#!/usr/bin/env python3

import os, re
import math, time
import sys
import argparse
import subprocess
import json


parser = argparse.ArgumentParser(description='This script splits tasks in multiple parallel jobs')

parser.add_argument("-l",  "--label",        required=True,  type=str, help="job label")
parser.add_argument("-b",  "--baseFolder",   required=True,  type=str, help="base folder")
parser.add_argument("-e",  "--exeName",      required=True,  type=str, help="absolute path of executable")
parser.add_argument("-s",  "--submit",                                 help="submit jobs", action='store_true')
parser.add_argument("-v",  "--verbose",        		               help="increase output verbosity", action='store_true')
parser.add_argument("-p",  "--numParal",     required=True, type=int,  help='number of parallel jobs, 0 for all')

args = parser.parse_args()

print('\nSTART\n')


currDir = os.getcwd()

print

try:
   subprocess.check_output(['mkdir',args.baseFolder+'/scripts/jobs'])
except subprocess.CalledProcessError as e:
  print(e.output)
try:
   subprocess.check_output(['mkdir',args.baseFolder+'/scripts/jobs/'+args.label])
except subprocess.CalledProcessError as e:
   print(e.output)
      
   
parallelCommand = "parallel --bar --jobs "+str(args.numParal)+" --results "+args.baseFolder+"/scripts/jobs/"+args.label+"/ "+args.baseFolder+"/"+args.exeName+" ::: "


##### creates directory and file list for job #######
jobDir = args.baseFolder+'/scripts/jobs/'+args.label+'/'
os.system('mkdir '+jobDir)
os.chdir(jobDir)

with open('/home/cmsdaq/LEGEND/OuterLArSim/scripts/tasks.json', 'r') as infile:
   tasks = json.load(infile)

for task in tasks:
   ##### creates config file #######
   print(task,tasks[task]['WLSConc'])
   with open(args.baseFolder + '/cfg/'+str(tasks[task]['cfg'])) as fi:
      contents = fi.read()
      configFileName = jobDir+'/config_%s.cfg'%task
      with open(configFileName, "w") as fo:
         fo.write(contents)
      command = 'sed -i \"s%^outfile .*$%outfile = '+jobDir+'/out_'+task+'.root'+'%\" '+configFileName
      os.system(command)
      command = 'sed -i \"s%^lightGuide_WLSConc = .*$%lightGuide_WLSConc = '+tasks[task]['WLSConc']+'%\" '+configFileName
      os.system(command)
      command = 'sed -i \"s%^det_conf.*$%det_conf = '+tasks[task]['det_conf']+'%\" '+configFileName
      os.system(command)
      command = 'sed -i \"s%^det_N.*$%det_N = '+tasks[task]['det_N']+'%\" '+configFileName
      os.system(command)
      
      parallelCommand += configFileName + " "
      
print(parallelCommand)
        

##### creates job file #######
with open(jobDir+'/jobs.sh', 'w') as fout:
   fout.write("#!/bin/sh\n")
   fout.write("echo\n")
   fout.write("echo 'START---------------'\n")
   fout.write("echo 'current dir: ' ${PWD}\n")
   fout.write("cd "+str(args.baseFolder)+"\n")
   fout.write("echo 'current dir: ' ${PWD}\n")
   fout.write("source ~/bin/myGeant4.sh\n")
   fout.write(parallelCommand+"\n")
   fout.write("echo 'STOP---------------'\n")
   fout.write("echo\n")
   fout.write("echo\n")
   
if args.submit:
   os.system("source "+jobDir+"/jobs.sh")

print('\nEND\n')

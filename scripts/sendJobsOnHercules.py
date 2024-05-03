#!/usr/bin/env python
import os, re
import commands
import math, time
import sys
import argparse
import subprocess
import random

from itertools import chain




# ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

if __name__ == '__main__':
   
   parser = argparse.ArgumentParser(description='This script splits tasks in multiple jobs and sends them on htcondor')
   parser.add_argument("-l",  "--label",          required=True,         type=str,   help="job label")
   parser.add_argument("-b",  "--baseFolder",     required=True,         type=str,   help="base folder")
   parser.add_argument("-e",  "--exeName",        required=True,         type=str,   help="absolute path of executable")
   parser.add_argument("-o",  "--outputFileName", required=True,         type=str,   help="outputFileName")
   parser.add_argument("-O",  "--outputFolder",   required=True,         type=str,   help="outputFolder")
   parser.add_argument("-c",  "--configFile",     required=True,         type=str,   help="config file")
   parser.add_argument("--crystalz",              required=True,         type=str,   help="crystal thickness")
   parser.add_argument("-q",  "--queue",          default="tomorrow",    type=str,   help="htcondor queue to use")
   parser.add_argument("-s",  "--submit",                                            help="submit jobs", action='store_true')
   parser.add_argument("-v",  "--verbose",                                           help="increase output verbosity", action='store_true')
   args = parser.parse_args()
   
   
   print 
   print 'START'
   print 
   
   currDir = os.getcwd()
   
   if not os.path.exists('jobs/'+args.label):
      os.makedirs('jobs/'+args.label)
      
   ##### creates directory and file list for job #######
   jobDir = currDir+'/jobs/'+args.label+'/'
   os.chdir(jobDir)
   
   ##### creates config file #######
   with open(args.baseFolder+'/'+args.configFile) as fi:
      contents = fi.read()
      configFileName = "config_"+args.label+".cfg"
      with open(jobDir+"/"+configFileName, "w") as fo:
         fo.write(contents)
      command = 'sed -i \"s%^seed .*$%seed = '+str(random.randrange(1,999999,1))+'%\" '+jobDir+'/'+configFileName
      os.system(command)
      command = 'sed -i \"s%^crystal_z .*$%crystal_z = '+args.crystalz+'%\" '+jobDir+'/'+configFileName
      os.system(command)
      command = 'sed -i \"s%^outFileName .*$%outFileName '+args.outputFileName+'_'+args.label+'%\" '+jobDir+'/'+configFileName
      os.system(command)
      
   ##### creates jobs #######
   with open('job_'+args.label+'.sh', 'w') as fout:
      fout.write("#!/bin/sh\n")
      fout.write("echo\n")
      fout.write("echo 'START---------------'\n")
      fout.write("echo 'current dir: ' ${PWD}\n")
      fout.write("cd "+str(args.baseFolder)+"\n")
      fout.write("echo 'current dir: ' ${PWD}\n")
      fout.write("source ~/bin/myGeant4.sh\n")
      fout.write("./"+args.exeName+" "+jobDir+"/"+configFileName+" "+args.outputFolder+"/"+args.outputFileName+"\n")
      fout.write("echo 'STOP---------------'\n")
      fout.write("echo\n")
      fout.write("echo\n")
      fout.close()
      os.system("chmod 755 job_"+args.label+".sh")
      
   #### creates htcondor job ######
   with open('htcondor_job_'+args.label+'.sub', 'w') as fout:
      fout.write("executable            = %s/job_%s.sh\n"%(jobDir,args.label))
      fout.write("arguments             = $(ClusterId) $(ProcId)\n")
      fout.write("output                = %s/job_%s.$(ClusterId).$(ProcId).out\n"%(jobDir,args.label) )
      fout.write("error                 = %s/job_%s.$(ClusterId).$(ProcId).err\n"%(jobDir,args.label) )
      fout.write("log                   = %s/job_%s.$(ClusterId).log\n"%(jobDir,args.label) )
      fout.write("transfer_output_files = \"\"\n" )
      fout.write("+JobFlavour           = \"%s\"\n"%(args.queue) )
      fout.write("queue \n")
      fout.close()
      
   ###### sends bjobs ######
   if args.submit:
      command = "condor_submit "+jobDir+"/htcondor_job_"+args.label+".sub"
      print command
      os.system(command)
      print "job " + str(args.label) + " submitted"
      
   os.chdir("../..")
   
   print
   print "your jobs:"
   os.system("condor_q")
   print
   print 'END'
   print

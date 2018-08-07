from array import array
from optparse import OptionParser
from optparse import OptionGroup
import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
import sys
import os
import stat
import subprocess
import time
import shutil
import imp

ROOT.gROOT.SetBatch(True)
ROOT.gDirectory.cd('PyROOT:/')

directory = os.path.dirname(os.path.abspath(sys.argv[0]))
basefolder = os.path.abspath(os.path.join(directory, "base"))

if not basefolder in sys.path:
    sys.path.append(basefolder)

pathToCMSSWsetup="/nfs/dust/cms/user/swieland/Darkmatter/CombineStuff/unfoldedlimits/setupCMSSW_8_1_0.txt"



usage = "usage: %prog [options]"
parser = OptionParser(usage = usage)


parser.add_option("--generateOnly",
dest="generateOnly",
action="store_true",
help="only generate Script, don't automatically submit them",
metavar = "generateOnly",
default = False
)

parser.add_option("--SamplePath",
dest="SamplePath",
action="store_true",
help="Path of Samples, default=/nfs/dust/cms/user/swieland/Darkmatter/ntuples_signal_madgraph_tagging",
metavar = "SamplePath",
default = "/nfs/dust/cms/user/swieland/Darkmatter/ntuples_signal_madgraph_tagging"
)

(options, args) = parser.parse_args()
SamplePath = options.SamplePath
generateOnly = options.generateOnly


axialsamples = []
vectorsamples = []
pseudosamples = []
scalarsamples = []

listnames = ["Axial", "Vector", "Pseudo", "Scalar"]
lists = [axialsamples, vectorsamples, pseudosamples, scalarsamples]

for x in os.listdir(SamplePath):
    for i,listname in enumerate(listnames):
        if x.startswith(listname):
            lists[i].append(x)
# print axialsamples

os.system("rm /nfs/dust/cms/user/swieland/Darkmatter/DM_Unfolding/rootfiles/signals.root")
os.system("mkdir -p rootfiles/signals")

commands=[]

script = """#!/bin/bash
cd /nfs/dust/cms/user/swieland/Darkmatter/CMSSW_8_0_26_patch2/src
eval `scramv1 runtime -sh`
cd -
"""

samplescript = ""


print len(lists)
for samples, listname in zip(lists, listnames):
    for i, sample in enumerate(samples):
        samplescript += "echo '" + sample + "'\n"
        samplescript += """root -l -b -q '/nfs/dust/cms/user/swieland/Darkmatter/DM_Unfolding/processSignalSamples.C(\"""" + sample + """\","/nfs/dust/cms/user/swieland/Darkmatter/DM_Unfolding/rootfiles/signals.root" )'\n"""
        directory = "/nfs/dust/cms/user/swieland/Darkmatter/DM_Unfolding/scripts/"
        filename = directory+"Job_"+listname+str(i)+".sh"
        if not os.path.exists(directory):
            os.makedirs(directory)
        f = open(filename, 'w')
        f.write(script+samplescript)
        f.close()
        samplescript = ""
        print filename
        commands.append(filename)

print "created scripts in ", directory


if not generateOnly:
    submitstring = "python submit.py -a SignalsArrayJob.sh " 
    for command in commands:
        submitstring += command
        submitstring += " "
    os.system(submitstring)  
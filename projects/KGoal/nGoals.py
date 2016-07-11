#!/usr/bin/python3

# Some utilities for dealing with result files:
# colrm
# sort
# meld

import os
import sys
    
configs = ['min.h', 'max.h', 'pergoal.h', 'uniform.h']
nGoals = [1, 2, 4, 8, 16, 32]

for c in configs:
    for n in nGoals:
        cmd = """(make run MODE=production CPP=test.cpp CONFIG="projects/KGoal/Config/{c}" OPT="-s ost001d.map8 -i ost001d_100_128goals.inst --nGoals {n} {h} --prefixTitle '   Config  nGoals' --prefixData '{c!s:>9}  {n!s:>6}'")""".format(c=c, n=n, h="" if n == 1 and c == 'min.h' else ' --hideTitle')
        #print(cmd)
        os.system(cmd)

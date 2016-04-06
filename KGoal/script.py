#!/usr/bin/python3

# Some utilities for dealing with result files:
# colrm
# sort
# meld

import os
    
execs = ["TestMinHeuristic", "TestPerGoal", "TestUniformSearch"]
#execs = ["TestUniformSearch"]
nGoals = [1, 2, 4, 8, 16, 32]

for e in execs:
    print("\n" + e[4:])
    for n in nGoals:
        cmd = "./{e} -i ost001d.txt  -s ost001d.map8 --nGoals {n} --prefixTitle nGoals --prefixData {n}". \
              format(e=e, n=n)
        cmd += ("" if n == 1 else " --hideTitle")
        #print(cmd)
        os.system(cmd)

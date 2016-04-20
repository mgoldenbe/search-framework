#!/bin/sh

make production CONFIG="ConfigMinHeuristic.h"; cp Test TestMinHeuristic
make production CONFIG="ConfigPerGoal.h"; cp Test TestPerGoal
make production CONFIG="ConfigUniformSearch.h"; cp Test TestUniformSearch

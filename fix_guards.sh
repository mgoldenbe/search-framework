#!/bin/sh

# Fixes include guards.
# The assumption is that all files already have include guards that end in _H.
for file in `find | grep "\.h$" | grep -v outside`; do
    echo $file
    guard=slb_`echo $file | sed -e 's/[\.\/]/_/g' | colrm 1 2`
    guard=`echo $guard | tr [a-z] [A-Z]`
    sed -i "s/[A-Z0-9_]*_H\s*$/$guard/g" $file
done

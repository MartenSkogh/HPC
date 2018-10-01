#!/usr/bin/env bash

for i in {1..10}
do
    cp -r /usr/include /run/mount/scratch/cp_dir/sdd_include$i
done

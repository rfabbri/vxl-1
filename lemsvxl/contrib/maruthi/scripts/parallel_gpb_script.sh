#!/bin/bash

dir=$1
arg=\'$dir\'

command="matlab -nosplash -nodesktop -nojvm -r \"parallel_compute_gpb_edges($arg);quit\";"
eval $command
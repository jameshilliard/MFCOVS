#!/bin/bash
cd `dirname $0`
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD

./a.out

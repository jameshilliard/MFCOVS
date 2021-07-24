#!/bin/bash
datetime=`date +%FT%T`

echo "-------------------------------------"
echo "git add -A"
echo "-------------------------------------"
git add -A

echo "-------------------------------------"
echo "git status"
echo "-------------------------------------"
git status

echo "-------------------------------------"
echo "git commit -m ${datetime} "
echo "-------------------------------------"
git commit -m "${datetime}"

echo "-------------------------------------"
echo "git push"
echo "-------------------------------------"
git push
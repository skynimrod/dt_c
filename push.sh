#!/bin/sh
time=`date "+%Y%m%d"`
echo "${time}"

git add .;
# 
if [ -n "$1" ]; then  
    echo "commit $1!";
    git commit -m "$1";
else
    git commit -m "${time}";
fi

git push -u origin master;
git push -u origin master;
echo "end!"
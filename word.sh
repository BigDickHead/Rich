#!/bin/bash
trap "exit 1" 1 2 15

DICT=/Dropbox/Rich/blog/Eng/GRE.txt

cat $DICT | while read oneline
do
    echo $oneline | tr ' ' '\n'
    /usr/share/stardict/speakall `echo $oneline | awk '{print $1}'`
    clear
done

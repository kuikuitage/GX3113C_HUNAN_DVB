#!/bin/bash

echo '<?xml version="1.0" encoding="ISO-8859-1" standalone="yes"?>'>./image.xml
echo "<images>">>./image.xml
#find *.bmp *.jpg  | awk '{print "\t<image id=\"" $0 "\">" $0 "</image>"}'>>./image.xml
#echo "</images>">>./image.xml

NAME=`find . -name "*.[bj][mp]?"`
for i in $NAME ; do
	ID=${i##*/}
	FILE=${i#*/}
	echo '	<image id= "'$ID'" >'$FILE'</image>'>>./image.xml
done

echo "</images>">>./image.xml

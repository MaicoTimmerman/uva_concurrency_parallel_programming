#!/bin/bash
LIB=lib
SRC=src
CLASSPATH=$(echo "$LIB"/*.jar | tr ' ' ':')
PACKAGE_NAME=nl.uva
MAIN_CLASS=$PACKAGE_NAME.AssignmentMapreduce

javac -cp $CLASSPATH -d . $SRC/nl/uva/*.java


echo "Main-Class: $MAIN_CLASS" > manifest
echo "Class-Path: " >> manifest

for f in $LIB/*.jar
do
  echo "Processing $f file..."
  echo " $f " >> manifest
done


jar cvfm $MAIN_CLASS.jar manifest nl/uva/*.class lib

#!/bin/bash
for x in *
do 
        echo $x
        wc -l $x
done
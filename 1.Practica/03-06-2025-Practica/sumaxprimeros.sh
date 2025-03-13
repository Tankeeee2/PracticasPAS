#!/bin/bash
#Programa que imprime la suma de los x primeros numeros
read -p "Introduzca un numero : " x
let i=0; let sum=0

while [ $i -le $x ];
do
        let sum=$sum+$i
        let i=$i+1
done
echo "La suma es $sum"
#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Uso: $0 <directorio>"
    exit
fi

dir=$1 

if [ ! -d "$dir" ]; then # -d: saber si es un directorio
    echo "El directorio '$dir' no existe."
    exit
fi

ficheros=$(find "$dir" -type f -name "*") 


if [ -z "$ficheros" ]; then #-z: compara que $ficheros no esté vacía
    echo "No se encontraron ficheros de texto en el directorio '$dir'."
    exit
fi

# N Lineas en el primer fichero
num_lineas=$(wc -l < "$(echo "$ficheros" | head -n 1)")

for ((i = 1; i <= num_lineas; i++)); do
    suma=0
    while IFS= read -r fichero; do  
        valor=$(sed -n "${i}p" "$fichero")
        suma=$((suma + valor))
    done <<< "$ficheros"
    echo "Asistentes a la sesión $i: $suma"
done
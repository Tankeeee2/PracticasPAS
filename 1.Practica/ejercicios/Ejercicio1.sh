#!/bin/bash

# Comprobar si se ha proporcionado 1 argumento, $# argn, $0 argv[0]
if [ $# -ne 1 ]; then  
    echo "Uso: $0 <directorio>"
    exit
fi

# Obtener el directorio de los ficheros , argv[1]
dir=$1 

# Comprobar si el directorio existe, -d
if [ ! -d "$dir" ]; then 
    echo "El directorio '$dir' no existe."
    exit
fi

# Buscar cualquier (-name"*") archivo regular, (-type f) de dir en ficheros
ficheros=$(find "$dir" -type f -name "*") 

# Comprobar si hay al menos un fichero en el directorio (-z compara que un string no esté vacío)
if [ -z "$ficheros" ]; then 
    echo "No hay ficheros de texto en '$dir'."
    exit
fi

# Obtiene el número de líneas (wc -l) en el primer fichero ( head -n 1 (primera linea (fichero) de $ficheros)) 
num_lineas=$(wc -l < "$(echo "$ficheros" | head -n 1)")


#TO   DO //////Hacer que verifique que todos los archivos tengan mismo n de lineas


# Iterar sobre cada línea de los ficheros y sumar los valores correspondientes
# sed: realiza transformaciones de texto a num.
# -n:  Es una opción de sed que suprime la salida de las líneas de entrada que sean especificadas
# i= linea a leer
# ${i}p:  sed lee la línea  i. 
for ((i = 1; i <= num_lineas; i++)); do
    suma=0
    while IFS= read -r fichero; do
        var=$(sed -n "${i}p" "$fichero")
        suma=$((suma + var))
    done <<< "$ficheros"
    echo "Asistieron $suma a la sesión $i"
done
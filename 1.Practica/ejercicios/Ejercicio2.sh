#!/bin/bash

# Comprobar  argumentos
if [ "$#" -ne 4 ]; then
    echo "Uso: $0 <directorio_origen> <directorio_destino> <comprimir(0|1)> <sobreescribir(0|1)>"
    exit 1
fi

origen="$1"
destino="$2"
comprimir="$3"
sobreescribir="$4"

# Comprobar si existe el origen
if [ ! -d "$origen" ]; then
    echo "El directorio de origen '$origen' no existe."
    exit 1
fi

# Crear el directorio de destino
if [ ! -d "$destino" ]; then
    mkdir -p "$destino"
fi


if [ "$comprimir" -ne 0 ] && [ "$comprimir" -ne 1 ]; then
    echo "El valor para comprimir debe ser 0 o 1."
    exit 1
fi

if [ "$sobreescribir" -eq 0 ]; then
    echo "No se sobreescribirá el archivo"
    elif [ "$sobreescribir" -eq 1 ]; then
    echo "Se sobreescribirá el archivo"
    else
    echo "El valor para sobreescribir debe ser 0 o 1."
    exit 1
fi


# Obtener el directorio de origen (basename: nombre del fichero sin ruta ni extension)
directorio=$(basename "$origen")

# Nombre del usuario y fecha en el formato adecuado
usuario=$USER
fecha=$(date +"%Y%m%d")

# Nombre del archivo copia (-eq equal)
nombre_copia="${directorio}_${usuario}_${fecha}"
if [ "$comprimir" -eq 1 ]; then
    nombre_copia="${nombre_copia}.tar.gz"
else
    nombre_copia="${nombre_copia}.tar"
fi

# Comprobar si ya existe un archivo con el mismo nombre
ruta_destino="${destino}/${nombre_copia}"

if [ -e "$ruta_destino" ] && [ "$sobreescribir" -eq 0 ]; then
    echo "El archivo de copia '$nombre_copia' ya existe en el directorio de destino."
    exit 1
fi

# -c Crear archivo
# -z Comprimir
# -f  Especificar nombre salida
if [ "$comprimir" -eq 1 ]; then
    tar -czf "$ruta_destino" -C "$origen" .
    
else
    tar -cf "$ruta_destino" -C "$origen" .
fi

echo "La copia de seguridad se ha creado correctamente en '$ruta_destino'."
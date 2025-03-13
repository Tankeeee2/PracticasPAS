#!/bin/bash

# Obtener la fecha actual en el formato deseado (mes-dia-año)
fecha=$(date +"%m-%d-%Y")

# Nombre de la carpeta
nombre_carpeta="${fecha}-Practica"

# Crear la carpeta
mkdir "$nombre_carpeta"

# Confirmar la creación de la carpeta
echo "La carpeta '$nombre_carpeta' ha sido creada con éxito."

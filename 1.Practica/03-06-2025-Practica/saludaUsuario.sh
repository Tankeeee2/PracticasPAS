#!/bin/bash
#Herramientas: $USER, if [ condicion ], read -p
read -p "Introduce tu nombre de usuario: " nombre
if [ $nombre == $USER ];
then
        echo "Bienvenido/a \"$nombre\"!!"
else
    echo "Eso es mentira!"
fi
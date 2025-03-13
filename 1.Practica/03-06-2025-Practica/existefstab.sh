#!/bin/bash
if [ -f /etc/fstab ];
then
cp /etc/fstab .
echo " Existe mi bro, carpeta copiada aquí 👍."
else
echo " Archivo /etc/fstab no existe ."
exit 1
fi


#
#if [ $# -ne 1 ];
#then
#    echo "Debes pasar el nombre del directorio"
#    exit 1
#   fi

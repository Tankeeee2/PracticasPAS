#!/bin/bash

#Creamos directorio temporal de instalación
mkdir $HOME/tmp1
cd $HOME/tmp1

# Si por lo que sea la dirección que hay abajo no dispone del fichero, buscar otra con Google
wget -O httpd-2.4.63.tar.bz2 https://dlcdn.apache.org/httpd/httpd-2.4.63.tar.bz2
tar jxvf httpd-2.4.63.tar.bz2

# Bajamos y descomprimimos la última versión de las librerías APR
wget -O apr-1.6.5.tar.bz2 https://dlcdn.apache.org/apr/apr-1.6.5.tar.bz2
wget -O apr-util-1.6.3.tar.bz2 https://dlcdn.apache.org/apr/apr-util-1.6.3.tar.bz2
tar jxvf apr-1.6.5.tar.bz2
tar jxvf apr-util-1.6.3.tar.bz2

# Instalamos el compilador gcc
sudo apt-get update
sudo apt-get install build-essential

# Movemos las librerías a la carpeta de apache, para que éste las utilice
mv apr-1.6.5 ./httpd-2.4.63/srclib/apr
mv apr-util-1.6.3 ./httpd-2.4.63/srclib/apr-util

# Instalamos la libreria pcre
sudo apt-get install libpcre3 libpcre3-dev

# Instalamos la libreria expat
sudo apt-get install libexpat1-dev

#Compilamos apache
cd httpd-2.4.63
./configure --prefix=$HOME/httpd --with-included-apr
sudo make -j4
sudo make install

#Borrar el directorio temporal de instalación
sudo rm -rf $HOME/tmp1



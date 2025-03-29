#!/bin/bash
# ejercicio2.sh
# Este script formatea el fichero de libros (libros.txt) usando sed.

# Título: XXX
# | -> Autor: XXX
# | -> Año: XXX
# | -> Precio: XXX
# | -> Género: XXX

# Comprobación: se debe pasar exactamente un argumento.
if [ "$#" -ne 1 ]; then
    echo "Argumentos erróneos. Uso: $0 <fichero_libros>."
    exit 1
fi

file="$1"

# Comprobación: el fichero debe tener extensión .txt.
if [[ "$file" != *.txt ]]; then
    echo "Se esperaba un fichero del tipo txt."
    exit 1
fi

# Se utiliza sed para procesar el fichero de la siguiente manera:
# 1. '/^$/d' elimina las líneas vacías.
#    - Con '^$' se indica una línea que no tiene ningún carácter.
#    - La opción 'd' borra esas líneas.
#
# 2. '/^[-_]\+$/d' elimina las líneas que contienen solo guiones o subrayados.
#    - '^[-_]\+$' significa: la línea empieza (^) y termina ($) con uno o más (-_)\+ de estos caracteres.
#
# 3. Para las líneas que comienzan por "Autor:", "Año:", "Precio:" o "Género:",
#    usamos 's/^/| -> /' para anteponer "| -> " al inicio de la línea.
#
# Cada expresión se aplica en orden para obtener el formato deseado.
sed -e '/^$/d' \
    -e '/^[-_]\+$/d' \
    -e '/^Autor:/ s/^/| -> /' \
    -e '/^Año:/ s/^/| -> /' \
    -e '/^Precio:/ s/^/| -> /' \
    -e '/^Género:/ s/^/| -> /' \
    "$file" >> librosbien.txt

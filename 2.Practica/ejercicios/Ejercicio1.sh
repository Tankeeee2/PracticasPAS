#!/bin/bash
# ejercicio1

# Comprobamos que se pase exactamente un argumento al script.
if [ "$#" -ne 1 ]; then
    echo "Argumentos erróneos. Uso: $0 <fichero_libros>."
    exit 1
fi

file="$1"

# Verificamos que el fichero tenga extensión .txt.
if [[ "$file" != *.txt ]]; then
    echo "Se esperaba un fichero de tipo txt."
    exit 1
fi

echo "1) Años de los libros publicados entre 1950 y 2020:"
# Con grep -E buscamos líneas que contengan "Año: " seguido de un año.
# La parte (19[5-9][0-9]|20(0[0-9]|1[0-9]|20)) filtra años:
#   - 19[5-9][0-9] coincide con años desde 1950 hasta 1999.
#   - 20(0[0-9]|1[0-9]|20) coincide con años de 2000 a 2020.
# Luego, con sed extraemos solo el número del año eliminando el resto del texto.
grep -E "Año: (19[5-9][0-9]|20(0[0-9]|1[0-9]|20))" "$file" | sed -E 's/.*Año: ([0-9]{4}).*/\1/'
echo

echo "2) Precios superiores a 20 euros:"
# Con grep -oE extraemos los precios en formato "XX,XXe".
# La expresión se divide en dos partes:
#   - (20,(0[1-9]|[1-9][0-9])e): captura precios que empiezan por 20, pero solo si los centavos no son 00.
#   - (([2-9][0-9]+|[0-9]{3,}),[0-9]{2}e): captura precios que empiezan por números mayores a 20.
grep -oE "((20,(0[1-9]|[1-9][0-9])e)|(([2-9][0-9]+|[0-9]{3,}),[0-9]{2}e))" "$file"
echo

echo "3) Número de libros por género:"
# Con grep -oE extraemos cualquier cadena que esté entre corchetes: "\[[^]]+\]".
# Luego, con sed quitamos el primer "[" y el último "]" (sed 's/^\[//; s/\]$//').
# Después, usamos sort para ordenar las entradas y uniq -c para contar las repeticiones.
# Finalmente, con sed damos formato a la salida para que se muestre "género aparece X veces".
grep -oE "\[[^]]+\]" "$file" | sed 's/^\[//; s/\]$//' | sort | uniq -c | sed -E 's/^[[:space:]]*([0-9]+)[[:space:]]+(.*)/\2 aparece \1 veces/'
echo

echo "4) Palabras de al menos 8 caracteres que empiezan por consonante y terminan por vocal:"
# Con grep -oE buscamos palabras completas que cumplan:
#   - \b: indica el inicio de la palabra.
#   - [bcdfghjklmnpqrstvwxyzBCDFGHJKLMNPQRSTVWXYZ]: la palabra debe comenzar con una consonante.
#   - [A-Za-záéíóúÁÉÍÓÚ]{6,}: se requieren al menos 6 letras adicionales (para un mínimo de 8 caracteres).
#   - [aeiouáéíóúAEIOUÁÉÍÓÚ]: la palabra debe terminar en vocal.
#   - \b: marca el final de la palabra.
grep -oE '\b[bcdfghjklmnpqrstvwxyzBCDFGHJKLMNPQRSTVWXYZ][A-Za-záéíóúÁÉÍÓÚ]{6,}[aeiouáéíóúAEIOUÁÉÍÓÚ]\b' "$file"
echo

echo "5) Líneas de autor cuyo nombre o apellido contiene una doble \"ll\":"
# Con grep "^Autor: " filtramos solo las líneas que empiezan por "Autor:".
# Luego, con grep -i "ll" buscamos de forma insensible a mayúsculas aquellas líneas que contengan "ll".
grep "^Autor: " "$file" | grep -i "ll"
echo

echo "6) Títulos de libros con más de tres palabras:"
# Con grep "^Título:" seleccionamos las líneas que comienzan por "Título:".
# Dentro del while, con sed quitamos el prefijo "Título:" y los espacios para obtener solo el título.
# Luego, con grep -E se comprueba que el título contenga al menos 3 espacios (indicativo de 4 o más palabras).
grep "^Título:" "$file" | while read -r line; do
    # Quitamos "Título:" para quedarnos solo con el contenido del título.
    title=$(echo "$line" | sed 's/^Título:[[:space:]]*//')
    # Con grep -E buscamos que haya al menos tres grupos de caracteres seguidos de espacio.
    echo "$title" | grep -qE '(\S+[[:space:]]+){3,}\S+'
    if [ $? -eq 0 ]; then
        echo "$line"
    fi
done
echo

echo "7) Títulos de libros cuyo precio termina en ,99€:"
# Con grep -B 3 buscamos la línea que comience con "Precio:" y que termine en ".99€".
# La opción -B 3 muestra las tres líneas anteriores a la coincidencia, donde se encuentra el título.
# Luego, con grep "^Título:" filtramos solo la línea del título y con sed eliminamos líneas con "--".
grep -B 3 "^Precio:.*\,99€" "$file" | grep "^Título:" | sed '/^--$/d'
echo

echo "8) Número de libros con año de publicación anterior a 2000:"
# Con grep "^Año:" seleccionamos las líneas que empiezan por "Año:".
# Luego, con grep "Año: 19" quedamos solo con aquellos años que comienzan con "19" (es decir, anteriores al 2000).
# Finalmente, con wc -l contamos el número de líneas resultantes.
grep "^Año:" "$file" | grep "Año: 19" | wc -l
echo

echo "9) Líneas con dos o más palabras con mayúscula consecutivas:"
# Con grep -E buscamos en cada línea que, después del primer ":" y espacio,
# aparezcan al menos dos palabras que comiencen con una letra mayúscula.
grep -E ":[[:space:]]+.*\b[A-Z][a-zA-Z]*\b.*\b[A-Z][a-zA-Z]*\b" "$file"
echo

echo "10) Géneros con una palabra compuesta:"
# Con grep -oE extraemos los géneros escritos entre corchetes.
# Luego, con sed eliminamos los corchetes.
# Finalmente, con grep "-" nos quedamos solo con aquellos géneros que contienen un guion.
grep -oE "\[[^]]+\]" "$file" | sed 's/^\[//; s/\]$//' | grep "-"

#!/bin/bash
# ejercicio3

# Comprobamos que se hayan pasado dos argumentos.
if [ "$#" -ne 2 ]; then
    if [ "$#" -eq 0 ]; then
        echo "Por favor, proporciona el archivo de acceso."
    else
        echo "Por favor, proporciona la opción 'repite-si' o 'repite-no'."
    fi
    exit 1
fi

logfile="$1"
modo="$2"

# Validamos que el segundo argumento sea "repite-si" o "repite-no".
if [ "$modo" != "repite-si" ] && [ "$modo" != "repite-no" ]; then
    echo "Modo no válido. Usa 'repite-si' o 'repite-no'."
    exit 1
fi

# Con grep " 200 " buscamos líneas con "200",
# suponiendo que en el log aparece separado por espacios (por ejemplo, ... "GET /url HTTP/1.1" 200 ...).
grep " 200 " "$logfile" |
# Extraemos la dirección IP y la URL solicitada utilizando sed.
#   - ^\([^ ]*\)     : Con \([^ ]*\) capturamos la primera cadena sin espacios (la dirección IP).

#   - .*"            : Ignoramos hasta llegar a la primera comilla (donde inicia el método HTTP).
#   - \(GET\|POST\|HEAD\) : Indicamos que el método HTTP puede ser GET, POST o HEAD.
#   - [ ]            : Buscamos un espacio.
#   - \([^ ]*\)      : Con \([^ ]*\) capturamos la URL solicitada (cadena sin espacios).

#   - [ ]HTTP\/[0-9.]*": Ignoramos desde el espacio hasta la comilla que cierra la solicitud HTTP.
#   - .*             : Ignoramos el resto de la línea.
#   - La opción -n junto con el flag "p" hace que se imprima solo la línea modificada.

sed -n 's/^\([^ ]*\).*"\(GET\|POST\|HEAD\) \([^ ]*\) HTTP\/[0-9.]*".*/\1 \3/p' |
# Ordenamos las solicitudes por dirección IP utilizando sort.
sort > temp_result.txt

# Comprobamos el modo de salida.
if [ "$modo" = "repite-si" ]; then
    cat temp_result.txt
else
    # Utilizamos uniq -c para contar cuántas veces aparece cada línea.
    # Luego, con sed se reformatea la salida para que se muestre:
    # "dirección_ip URL --> N veces"
    uniq -c temp_result.txt | sed -E 's/^[[:space:]]*([0-9]+)[[:space:]]+(.*)/\2 --> \1 veces/'
fi


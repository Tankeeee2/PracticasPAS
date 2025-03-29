#!/bin/bash 

# Comprobar argumentos
if [ "$#" -ne 1 ]; then
    echo "Uso: $0 <directorio>"
    exit 1
fi

directorio="$1"

# Comprobar si existe el directorio (-d comprueba si es un directorio)
if [ ! -d "$directorio" ]; then
    echo "El directorio $directorio no existe."
    exit 1
fi

# Función para obtener la fecha de modificación en segundos desde Epoch
function obtener_fecha_modificacion {
    local archivo="$1"
    local fecha_modificacion=$(stat -c %Y "$archivo")
    # El comando stat se utiliza para mostrar información sobre el 
    # fichero dado, y -c %Y se usa para que la salida muestre en
    # segundos desde la Epoch, la ultima vez que se cambió el estado del archivo
    echo "$fecha_modificacion"
}

# Función tamaño
function obtener_tamano {
    local archivo="$1"
    local obtener_tamano=$(stat -c %s "$archivo")
    # -c %s: devolver tamaño del archivo en bytes
    echo "$obtener_tamano"
}

# Función para obtener la cadena de permisos del archivo
function obtener_permisos {
    local archivo="$1"
    local permisos=$(stat -c %A "$archivo")
    # -c %A: se utiliza para especificar el formato de salida deseado, que
     # en este caso es la cadena de permisos en formato legible por humanos.
    echo "$permisos"
}

# Obtener la lista de archivos (sin los subdirectorios) y ordenarlos por fecha de modificación
archivos=$(find "$directorio" -type f -printf "%f\t%p\t" -exec stat -c %Y:%s:%A {} \; | sort -n -t: -k1 | cut -f 2-)
# f: fichero
# Con la opción -printf, find se formatea la salida
 # (%f) nombre del archivo,(\t) tab , 
 # (%p) ruta del archivo, (/t) otro tabulador.
 # La salida es el nombre del archivo, la ruta completa y un tab entre ellos

# -exec: para cada archivo ejecuta el comando siguiente
# | sort -n -t: -k1: La salida se pasa a sort, que ordena
 # (-n)las líneas de entrada numéricamente, (-k1) en función del primer campo
 # (-t:) utilizando : como delimitador de campo . // Ordena por fecha de modificacion

# | cut -f 2-: muestra todos los campos a partir del segundo, quitando el nombre del archivo
 
# Mostrar la información de cada archivo
while IFS=$'\t' read -r nombre_archivo ruta_archivo fecha_modificacion obtener_tamano permisos; do
    echo -e "$nombre_archivo\t$ruta_archivo\t$fecha_modificacion\t$obtener_tamano bytes\t$permisos"
done <<< "$archivos"
# IFS=$'\t': Establece el separador de campo (IFS) como un tab
 # Indicando a read que utilice el tabulador como
 # separador de campos al leer y que cada
 # campo se almacene correctamente en cada variables.

# read -r: Lee una línea del texto de entrada y divide los campos
 # separados por tabuladores en las variables indicadas 
 # (nombre_archivo, ruta_archivo, fecha_modificacion, obtener_tamanio y permisos)
 
# echo -e: Imprime en la salida estándar una línea formateada que contiene
 # la información de cada archivo. Cada campo está separado por un
 # tabulador \t, y el tamaño del archivo está seguido por la cadena "bytes"
# <<< "$archivos": Utiliza la redirección para pasar el contenido de la
 # variable $archivos como entrada al bucle while.
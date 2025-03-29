#!/bin/bash

# Función cadena alfa
cadena_alfa(){
    local longitud="$1"
    local letras="abcdefghijklmnñopqrstuvwxyzABCDEFGHIJKLMNÑOPQRSTUVWXYZ"
    local cadena=""
    # local: las variables locales solo existen dentro de la función

    for (( i=0; i<longitud; i++ )); do
            letra="${letras:$((RANDOM % ${#letras})):1}"
            # Mete caracter a letra
            # De letras
            # Randomizandolo con el modulo de RANDOM % ${#letras}(long cadena)
            # Solo mete uno a partir de la Randomización

        cadena+="$letra"
    done

    echo "$cadena"
}

# Función cadena num
cadena_num() {
    local longitud="$1"
    local cadena=""

    for (( i=0; i<longitud; i++ )); do
        numero=$((RANDOM % 10)) 
        cadena+="$numero"
    done

    echo "$cadena"
}

# Función cadena alfanum
cadena_alfanum(){
    local longitud="$1"
    local cadena=""

    for (( i=0; i<longitud; i++ )); do
        tipo_caracter=$((RANDOM % 3))
        case $tipo_caracter in
            0) letra=$(echo {a..z} | tr -d ' ' | cut -c$((RANDOM % 26 + 1))) ;;
            1) letra=$(echo {A..Z} | tr -d ' ' | cut -c$((RANDOM % 26 + 1))) ;;
            2) letra=$((RANDOM % 10)) ;;

            # tr -d ' ': Elimina espacios en blanco 
            # cut -c$((RANDOM % 26 + 1)): Corta la secuencia de letras en una posición randomizada
        esac

        cadena+="$letra"
    done

    echo "$cadena"
}
# MAIN:

# Comprobamos argumentos
if [ $# -lt 1 ] || [ $# -gt 2 ]; then
    echo "Uso: $0 <longitud de la cadena> [tipo de cadena]"
    exit 1
fi

# Pedimos el tipo de cadena si no se ha proporcionado
if [ -z "$2" ]; then
    read -p "Introduce el tipo de cadena (alfanum, alfa o num): " tipo_cadena
else
    tipo_cadena="$2"
fi

longitud="$1"


# Generamos la cadena pedida
case $tipo_cadena in
    alfa) cadena=$(cadena_alfa "$longitud") ;;
    num) cadena=$(cadena_num "$longitud") ;;
    alfanum) cadena=$(cadena_alfanum "$longitud") ;;
    *) echo "Tipo de cadena no válido. Debe ser alfanum, alfa o num."
       exit 1 ;;
esac

# Imprimimos la cadena generada
echo "$cadena"

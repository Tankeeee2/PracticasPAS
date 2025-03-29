#!/bin/bash

# Comprobamos argumentos
if [ $# -ne 1 ]; then   
    echo "Uso: $0 <ruta_usuarios>"
    exit 1
fi

# Ruta al directorio (argv[1])
ruta_usuarios="$1"  



# Bucle que mete cada "ls" de ruta_usuarios en usuario, e itera en ellos
for usuario in $(ls "$ruta_usuarios"); do

    # Comprobar si existe el fichero de clave SSH 
    #(-f para saber si existe y si es un fichero)
    clave_ssh="$ruta_usuarios/$usuario/.ssh/id_rsa"    
    if [ -f "$clave_ssh" ]; then
        # Sacamos permisos del fichero id_rsa, directorio .ssh y home
        permisos_clave=$(stat -c %a "$clave_ssh")
        permisos_ssh=$(stat -c %a "$ruta_usuarios/$usuario/.ssh")
        permisos_home=$(stat -c %a "$ruta_usuarios/$usuario")
        # stat: para consultar los permisos de un fichero o directorio
        # stat -c permite personalizar la salida, %a representacion octal de los permisos (644 por ejemplo)


        # Verificar los permisos
        if [ "$permisos_clave" !="700" ] || [ "$permisos_ssh" != "700" ] || [ "$(echo "$permisos_home" | cut -c3)" != "0" ]; then
            echo "El usuario $usuario tiene una clave privada de SSH en $clave_ssh que no está protegida."
            echo "La clave debe ser accesible únicamente por el propietario."
            echo

            # Crear archivo de advertencia
            echo "Por favor, asegúrate de que los permisos de tu clave privada SSH estén configurados correctamente." > "$ruta_usuarios/$usuario/Desktop/AVISO_CLAVE_SSH.txt"
        fi
    fi
done
# r w x
# 4 2 1 0
#  653
#   7
# cut: Saca el tercer (en este caso) carácter 
# >: redirige la salida del echo al fichero de texto
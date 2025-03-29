#!/bin/bash
# ejercicio4.sh

# 1) Usuarios cuyo nombre empieza con "l":
echo "1) Usuarios cuyo nombre empieza con la letra 'l':"
# Con grep -E usamos la expresión ^l para seleccionar las líneas que comienzan con "l".
# Luego, con sed 's/:.*//' eliminamos desde el primer ":" en adelante, quedándonos solo con el nombre de usuario.
grep -E '^l' /etc/passwd | sed 's/:.*//'
echo

# 2) Usuarios con shell válido (diferente de /bin/false o /usr/bin/nologin):
echo "2) Usuarios con shell válido:"
# Con grep -vE excluimos las líneas que terminan (el $ indica fin de línea) en "/bin/false" o "/usr/bin/nologin".
# La expresión regular '(:/bin/false|:/usr/bin/nologin)$' identifica esas líneas.
# Luego, con sed 's/:.*//' extraemos el primer campo (nombre de usuario).
grep -vE '(:/bin/false|:/usr/bin/nologin)$' /etc/passwd | sed 's/:.*//'
echo

# 3) UID de los usuarios cuyo directorio home no está en /home:
echo "3) UID de los usuarios cuyo directorio home no está en /home:"
# Primero, con grep -v ':/home/' filtramos las líneas que -NO- contienen "/home/" (en el campo home).
# Luego, con sed extraemos el UID, que es el tercer campo.
# La expresión 's/^[^:]*:[^:]*:\([^:]*\):.*/\1/' se explica así:
#   - ^[^:]*:   -> desde el inicio hasta el primer ":" (nombre de usuario).
#   - [^:]*:    -> el segundo campo (generalmente "x" o la contraseña encriptada).
#   - \([^:]*\) -> captura el tercer campo (UID) hasta el siguiente ":".
#   - :.*       -> ignora el resto de la línea.
grep -v ':/home/' /etc/passwd | sed 's/^[^:]*:[^:]*:\([^:]*\):.*/\1/'
echo

# 4) Usuarios con GID mayor que 1000:
echo "4) Usuarios con GID mayor que 1000:"
# Con grep -E ':[0-9]{4,}:' seleccionamos líneas donde el GID tenga al menos 4 dígitos,
# lo que generalmente indica que es mayor o igual a 1000.
# Luego, con grep -v ":1000:" se eliminan las líneas con GID exactamente igual a 1000,
# dejando solo aquellas con un GID mayor que 1000.
# Por último, con sed 's/:.*//' extraemos el nombre de usuario (primer campo).
grep -E ':[0-9]{4,}:' /etc/passwd | grep -v ":1000:" | sed 's/:.*//'
echo

# 5) Usuarios y su UID con una ',' en su campo gecos:
echo "5) Usuarios y su UID con una ',' en su gecos:"
# El campo gecos es el quinto en /etc/passwd.
# La expresión regular '^[^:]*:[^:]*:[^:]*:[^:]*:[^,]*,[^:]*:' se interpreta de la siguiente forma:
#   - ^[^:]*:      -> desde el inicio hasta el primer ":" (usuario).
#   - [^:]*:       -> segundo campo.
#   - [^:]*:       -> tercer campo (UID).
#   - [^:]*:       -> cuarto campo (GID).
#   - [^,]*,[^:]*: -> quinto campo que contiene una coma (algún texto, luego una coma y más texto).
# Luego, con sed 's/^\([^:]*\):[^:]*:\([^:]*\):.*/\1, \2/':
#   - \([^:]*\) captura el nombre de usuario (primer campo).
#   - \([^:]*\) captura el UID (tercer campo).
# Finalmente, se muestra el usuario y UID separados por una coma.
grep -E '^[^:]*:[^:]*:[^:]*:[^:]*:[^,]*,[^:]*:' /etc/passwd | sed 's/^\([^:]*\):[^:]*:\([^:]*\):.*/\1, \2/'

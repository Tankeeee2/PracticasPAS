#!/bin/bash
# Ejercicio 5: 

# Obtener la lista de interfaces de red.

 
#   sed-n: Solo se imprimen las líneas que se le indiquen.
#    's/^\([a-z0-9]\+\):.*/\1/p':
#        s///: Hace que sed sustituya.
#        ^: Indica el comienzo de la línea.
#        [a-z0-9]: Captura una o más letras minúsculas o dígitos.
#         - \+: El signo '+' se busca uno o más de esos caracteres.
#        :: Después del grupo de captura se espera un carácter dos puntos ":".
#        .*\: El resto de la línea (.* significa cualquier carácter, 0 o más veces).
#        \1: Se reemplaza la línea entera por el contenido del primer grupo capturado( el nombre de la interfaz).
#        p: Imprime la línea resultante (esto es necesario porque usamos -n).
interfaces=$(ifconfig | sed -n 's/^\([a-z0-9]\+\):.*/\1/p')


for iface in $interfaces; do
  echo "Interfaz: $iface"
  iface_info=$(ifconfig "$iface")
  
  # Extraer la dirección IP asignada a la interfaz.

  # Comando usado:
  #   grep "inet addr:" Busca las líneas que contengan "inet addr:".

  #   * s/.*inet addr:\([0-9.]\+\).*/\1/
  #        .*: CCualquier texto antes de "inet".
  #        \([0-9.]\+\): Grupo que se captura (IP que buscamos)
  #        .*\: Resto de la línea después de la IP.
  #        \1: Sustituye toda la línea por el contenido del primer grupo (la IP).
  ip=$(echo "$iface_info" | grep "inet " | sed 's/.*inet \([0-9.]\+\).*/\1/')
  
  
  # Se muestra la dirección IP encontrada o un mensaje si no se asignó.
  echo "-> Dirección IP: ${ip:-No asignada}"
  
  # Extraer la dirección de broadcast y la máscara de red.

  # Se busca la línea que contenga "broadcast:"
  bcast=$(echo "$iface_info" | grep "broadcast" | sed 's/.*broadcast \([0-9.]\+\).*/\1/')
  
  
  # Ahora, para extraer la máscara de red:
  # Se busca la línea que contenga "netmask:" y se extrae la máscara.
  mask=$(echo "$iface_info" | grep "netmask" | sed 's/.*netmask \([0-9.]\+\).*/\1/')
=
  

    #Convertir mascara a formato decimal
  if [ -n "$mask" ]; then
    #Se usa el punto como separador
    IFS=. read -r o1 o2 o3 o4 <<< "$mask"

    #Primero pasamos a binario
    bin1=$(echo "obase=2; ibase=10; $o1" | bc)
    bin2=$(echo "obase=2; ibase=10; $o2" | bc)
    bin3=$(echo "obase=2; ibase=10; $o3" | bc)
    bin4=$(echo "obase=2; ibase=10; $o4" | bc)
   
    # Esta función utiliza grep para extraer cada "1" y wc -l para contarlos.
    cuenta_binario() {
      echo "$1" | grep -o "1" | wc -l
    }
    
    ones1=$(cuenta_binario "$bin1")
    ones2=$(cuenta_binario "$bin2")
    ones3=$(cuenta_binario "$bin3")
    ones4=$(cuenta_binario "$bin4")
    # Se suman los unos de los cuatro octetos para obtener el total de bits en la máscara.
    total_ones=$((ones1 + ones2 + ones3 + ones4))
    echo "-> Máscara de red: $total_ones bits"
  else
    echo "-> Máscara de red: No disponible"
  fi
  
  # 5. Comprobar si la interfaz tiene el cable conectado.
  # Se utiliza ethtool para obtener información de la interfaz.
  # La salida de ethtool se almacena en la variable ethtool_out.
  ethtool_out=$(ethtool "$iface" 2>/dev/null)
  # Se redirige stderr a /dev/null para no mostrar errores en caso de que la interfaz no sea manejable por ethtool.
  #
  # Se busca la línea que contiene "Link detected:".
  # La expresión sed:
  #   sed 's/.*Link detected: \(.*\)/\1/'
  # funciona de la siguiente forma:
  #   * .*Link detected: : Ignora todo lo que haya antes de "Link detected:".
  #   * \(.*\): Captura todo lo que haya después, que será el valor (por ejemplo, "yes" o "no").
  #   * \1: Reemplaza toda la línea por el contenido capturado.
  link=$(echo "$ethtool_out" | grep "Link detected:" | sed 's/.*Link detected: \(.*\)/\1/')
  
  # Si el valor es "yes" se considera que el cable está conectado.
  if [ "$link" = "yes" ]; then
    echo "-> Cable conectado: Sí"
  else
    echo "-> Cable conectado: No"
  fi
  
  # 6. Extraer las velocidades soportadas y las anunciadas.
  # Se busca la línea que contiene "Supported link modes:" para obtener las velocidades soportadas.
  # La expresión sed:
  #   sed 's/.*://; s/^[ \t]*//; s/[[:space:]]\+/, /g'
  #
  # Desglose:
  #   * s/.*:// elimina todo hasta el último ":" de la línea, dejando solo lo que viene después.
  #   * s/^[ \t]*// elimina los espacios o tabulaciones iniciales.
  #   * s/[[:space:]]\+/, /g reemplaza una o más ocurrencias de espacios en blanco por una coma seguida de un espacio.
  supported=$(echo "$ethtool_out" | grep "Supported link modes:" | sed 's/.*://; s/^[ \t]*//; s/[[:space:]]\+/, /g')
  if [ -z "$supported" ]; then
    supported="No disponible"
  fi
  echo "-> Velocidades soportadas: $supported"
  
  # Se realiza un proceso similar para extraer las velocidades anunciadas.
  advertised=$(echo "$ethtool_out" | grep "Advertised link modes:" | sed 's/.*://; s/^[ \t]*//; s/[[:space:]]\+/, /g')
  if [ -z "$advertised" ]; then
    advertised="No disponible"
  fi
  echo "-> Velocidades anunciadas como disponibles: $advertised"
  
  # Se imprime una línea en blanco para separar la información de cada interfaz.
  echo ""
done

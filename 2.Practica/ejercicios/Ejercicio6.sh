#!/bin/bash
# Ejercicio 6: Análisis del historial de comandos (últimas 20 líneas)


historial="historial.txt"
touch "$historial"

# Con tail se extraen las 20 últimas líneas del archivo de historial.
tail -n 20 ~/.zsh_history > "$historial"


#   sed 's/^[[:space:]]*\([^[:space:]]\+\).*/\1/'

#    ^[[:space:]]*       : Elimina (coincide con) cualquier espacio al principio.
#    \([^[:space:]]\+\)  : Captura (entre paréntesis) uno o más caracteres que no sean espacios
#                          (esto es la primera palabra, el nombre del comando).
#    .*                  : El resto de la línea (que descartamos).
#    \1                  : Reemplaza la línea completa por la primera palabra capturada.
grep "" "$historial" | sed 's/^[[:space:]]*\([^[:space:]]\+\).*/\1/' > cmds.txt


unique_cmds=$(sort cmds.txt | uniq)


for cmd in $unique_cmds; do
  # Contar cuántas líneas del fichero comienzan con el comando.
  count=$(grep -c "^$cmd" "$historial")
  
  max_args=0
  
  
  #   sed 's/^[[:space:]]*\([^[:space:]]\+\)[[:space:]]*//'

  #   ^[[:space:]]*            : Elimina espacios al inicio.
  #   \([^[:space:]]\+\)       : Coincide con la primera palabra (el comando) y la captura.
  #   [[:space:]]*             : Elimina los espacios que siguen al comando.
  #   //                       : Reemplaza todo lo anterior por nada, dejando solo los argumentos.
  #
  # Se recorre cada línea que comienza con el comando.
  while IFS= read -r line; do
    args=$(echo "$line" | sed 's/^[[:space:]]*\([^[:space:]]\+\)[[:space:]]*//')
    # Para contar el número de argumentos, aprovechamos que el shell puede separar
    # la variable en palabras (separadas por espacios).

    set -- $args  # Esto asigna cada palabra (argumento) a los parámetros posicionales.
    num_args=$#   # El número de argumentos es la cantidad de parámetros posicionales.
    
    # Si el número de argumentos de esta línea es mayor que el máximo hasta ahora, se actualiza.
    if [ "$num_args" -gt "$max_args" ]; then
      max_args=$num_args
    fi
  # Se filtran solo las líneas que comienzan exactamente con el comando buscado.
  done < <(grep "^$cmd" "$historial")
  
  # Mostrar el resultado para este comando.
  echo "Comando: $cmd"
  echo "-> Veces ejecutado: $count"
  if [ "$max_args" -eq 0 ]; then
    echo "-> Máximo número de argumentos:"
  else
    echo "-> Máximo número de argumentos: $max_args"
  fi
  echo ""
done

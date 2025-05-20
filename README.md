# PracticasPAS - Programación y Administración de Sistemas

Este repositorio contiene las prácticas realizadas para la asignatura de Programación y Administración de Sistemas del 2º curso del Grado en Ingeniería Informática de la Universidad de Córdoba.

## Descripción General

El proyecto incluye diferentes prácticas relacionadas con la programación en entornos POSIX, utilizando el lenguaje C para implementar soluciones que interactúan con el sistema operativo. Las prácticas abarcan temas como:

- Procesado de línea de comandos tipo POSIX
- Manejo de variables de entorno
- Obtención de información de usuarios y grupos del sistema
- Creación y gestión de procesos (fork y exec)
- Señales entre procesos
- Comunicación entre procesos (tuberías y colas de mensajes)

## Estructura de Carpetas

El repositorio está organizado en las siguientes carpetas principales:

- **1.Practica/**: Contiene los ejercicios y ejemplos de la primera práctica.
  - `ejemplosPresentacion/`: Ejemplos mostrados en clase.
  - `ejercicios/`: Implementaciones de los ejercicios propuestos.

- **2.Practica/**: Contiene los ejercicios y ejemplos de la segunda práctica.
  - `ejercicios/`: Implementaciones de los ejercicios propuestos.

- **3.Practica/**: Contiene los ejercicios y ejemplos de la tercera práctica.
  - `4-linea-de-comandos/`: Ejemplos de procesamiento de línea de comandos.
  - `5-variables-entorno/`: Ejemplos de manejo de variables de entorno.
  - `6-informacion-usuario/`: Ejemplos de obtención de información de usuarios.
  - `8-procesos/`: Ejemplos de creación y gestión de procesos.
  - `9-seniales/`: Ejemplos de manejo de señales entre procesos.
  - `10-pipes/`: Ejemplos de comunicación mediante tuberías.
  - `10-colas/`: Ejemplos de comunicación mediante colas de mensajes.

- **4.Practica/**: Contiene los ejercicios y ejemplos de la cuarta práctica.

## Instrucciones de Instalación y Uso

### Requisitos Previos

- Sistema operativo compatible con POSIX (Linux, macOS, etc.)
- Compilador GCC
- Bibliotecas de desarrollo estándar de C

### Compilación de los Ejemplos

Para compilar los ejemplos y ejercicios, navega hasta el directorio correspondiente y utiliza el compilador GCC:

```bash
gcc -o nombre_ejecutable archivo_fuente.c
```

Para los ejemplos que utilizan colas de mensajes, es necesario incluir la biblioteca de tiempo real:

```bash
gcc -o nombre_ejecutable archivo_fuente.c -lrt
```

### Ejecución

Una vez compilado, puedes ejecutar los programas de la siguiente manera:

```bash
./nombre_ejecutable [opciones]
```

Consulta la documentación específica de cada ejercicio para conocer las opciones disponibles.

## Prácticas

### Práctica 1
Introducción a la programación en entornos POSIX y manejo básico de archivos y directorios.

### Práctica 2
Profundización en la programación POSIX, incluyendo manejo de permisos y atributos de archivos.

### Práctica 3
Programación avanzada en POSIX, incluyendo:
- Procesado de línea de comandos
- Variables de entorno
- Información de usuarios y grupos
- Creación de procesos
- Señales entre procesos
- Comunicación entre procesos (tuberías y colas de mensajes)

### Práctica 4
Aplicación práctica de los conocimientos adquiridos en las prácticas anteriores.

## Autor

Juan José Cañete Gómez

## Licencia

Este proyecto está disponible para uso educativo y académico.

## Contacto

Para cualquier consulta o sugerencia, puedes contactar a través de:
- Email: jucago705@gmail.com
- GitHub: [Tankeeee2](https://github.com/Tankeeee2)

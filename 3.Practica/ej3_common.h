/**
 * Ejercicio 3: Archivo de cabecera común para cliente y servidor
 *
 * Este archivo contiene definiciones, constantes y funciones comunes utilizadas
 * tanto por el cliente como por el servidor en la implementación de comunicación
 * mediante colas de mensajes POSIX.
 *
 * Las colas de mensajes POSIX permiten la comunicación entre procesos no relacionados
 * (a diferencia de las tuberías que requieren procesos con un ancestro común).
 * Cada mensaje tiene un tamaño máximo definido y puede tener una prioridad asociada.
 */

#ifndef EJ3_COMMON_H
#define EJ3_COMMON_H

#include <stdio.h>     // Para funciones de entrada/salida estándar
#include <stdlib.h>    // Para funciones como exit(), getenv()
#include <string.h>    // Para funciones de manejo de cadenas
#include <mqueue.h>    // Para funciones de colas de mensajes POSIX (mq_*)
#include <sys/stat.h>  // Para constantes de modo de archivos (permisos)
#include <sys/types.h> // Para tipos como pid_t
#include <unistd.h>    // Para funciones POSIX básicas
#include <time.h>      // Para funciones de manejo de tiempo
#include <errno.h>     // Para códigos de error (errno)
#include <signal.h>    // Para manejo de señales

/**
 * Nombres base para las colas de mensajes
 *
 * En POSIX, los nombres de colas de mensajes deben comenzar con '/'
 * Estos nombres base se combinarán con el nombre de usuario para crear
 * nombres únicos para cada usuario en el sistema.
 */
#define SERVER_QUEUE "/server_queue" // Cola para enviar mensajes del cliente al servidor
#define CLIENT_QUEUE "/client_queue" // Cola para enviar mensajes del servidor al cliente

/**
 * Tamaño máximo de los mensajes
 *
 * Define el tamaño máximo en bytes que puede tener un mensaje.
 * Este valor debe ser consistente entre el cliente y el servidor.
 */
#define MAX_SIZE 1024

/**
 * Mensaje de salida
 *
 * Cadena que se enviará para indicar que se desea terminar la comunicación.
 * Cuando el cliente envía este mensaje, tanto el cliente como el servidor
 * deben finalizar de manera ordenada.
 */
#define MSG_EXIT "exit"

/**
 * Función: get_queue_name
 *
 * Genera un nombre único para una cola de mensajes basado en el nombre base
 * y el nombre de usuario actual. Esto evita conflictos cuando múltiples usuarios
 * ejecutan el programa en el mismo sistema.
 *
 * Parámetros:
 *   - queue_name: Buffer donde se almacenará el nombre generado
 *   - base_name: Nombre base de la cola (SERVER_QUEUE o CLIENT_QUEUE)
 *
 * En sistemas compartidos, es importante que cada usuario tenga sus propias
 * colas para evitar interferencias entre diferentes ejecuciones del programa.
 */
void get_queue_name(char *queue_name, const char *base_name)
{
    // Obtenemos el nombre de usuario de la variable de entorno USER
    char *username = getenv("USER");
    if (username == NULL)
    {
        // Si no se puede obtener el nombre de usuario, usamos "unknown"
        username = "unknown";
    }

    // Combinamos el nombre base con el nombre de usuario
    // Formato: /nombre_base-nombre_usuario
    sprintf(queue_name, "%s-%s", base_name, username);
}

/**
 * Función: funcionLog
 *
 * Registra mensajes en un archivo de log con marca de tiempo y también
 * los muestra por consola. Esta función es útil para depuración y para
 * mantener un registro de la actividad del programa.
 *
 * Parámetros:
 *   - mensaje: El mensaje a registrar
 *   - logFileName: Nombre del archivo de log
 *
 * El formato de cada entrada en el log es:
 * [YYYY-MM-DD HH:MM:SS] mensaje
 */
void funcionLog(const char *mensaje, const char *logFileName)
{
    FILE *file;
    time_t t;
    struct tm *tm_info;
    char timestamp[20]; // Buffer para la marca de tiempo

    // Obtenemos la hora actual del sistema
    time(&t);
    tm_info = localtime(&t); // Convertimos a hora local

    // Formateamos la marca de tiempo: YYYY-MM-DD HH:MM:SS
    strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", tm_info);

    // Abrimos el archivo de log en modo append (añadir al final)
    // Si el archivo no existe, se crea
    file = fopen(logFileName, "a");
    if (file == NULL)
    {
        perror("Error opening log file");
        return;
    }

    // Escribimos la marca de tiempo y el mensaje en el archivo
    fprintf(file, "[%s] %s\n", timestamp, mensaje);

    // Cerramos el archivo para liberar recursos y asegurar que se guarden los datos
    fclose(file);

    // También mostramos el mensaje por consola para seguimiento en tiempo real
    printf("%s\n", mensaje);
}

#endif /* EJ3_COMMON_H */

/**
 * Ejercicio 3: Servidor para comunicación mediante colas de mensajes POSIX
 *
 * Este programa implementa un servidor que recibe cadenas de texto desde un cliente,
 * cuenta el número de caracteres en cada cadena y envía el resultado de vuelta al cliente.
 * La comunicación se realiza mediante dos colas de mensajes POSIX:
 * - Una cola para recibir mensajes del cliente
 * - Otra cola para enviar respuestas al cliente
 *
 * El servidor es responsable de crear y eliminar ambas colas.
 */

#include "ej3_common.h" // Incluye definiciones y funciones comunes

/**
 * Nombre del archivo de log para el servidor
 *
 * Todas las acciones y eventos importantes se registrarán en este archivo
 * para facilitar la depuración y el seguimiento de la ejecución.
 */
#define LOG_FILE "log-servidor.txt"

/**
 * Variables globales para los descriptores de las colas de mensajes
 *
 * Se definen como globales para que puedan ser accedidas desde la función
 * de limpieza y el manejador de señales.
 *
 * - server_queue: Descriptor de la cola para recibir mensajes del cliente
 * - client_queue: Descriptor de la cola para enviar respuestas al cliente
 *
 * El valor inicial -1 indica que las colas no están abiertas.
 */
mqd_t server_queue = -1;
mqd_t client_queue = -1;

/**
 * Función: cleanup
 *
 * Realiza la limpieza de recursos antes de terminar el programa.
 * Cierra y elimina las colas de mensajes si están abiertas.
 *
 * Esta función se llama tanto al finalizar normalmente como
 * cuando se recibe una señal de terminación.
 */
void cleanup()
{
    char msgbuf[100]; // Buffer para mensajes de log

    // Cerramos la cola del servidor si está abierta
    if (server_queue != -1)
    {
        // mq_close cierra el descriptor de la cola pero no la elimina
        if (mq_close(server_queue) == -1)
        {
            sprintf(msgbuf, "Error al cerrar la cola del servidor: %s", strerror(errno));
            funcionLog(msgbuf, LOG_FILE);
        }
        else
        {
            funcionLog("Cola del servidor cerrada", LOG_FILE);
        }
    }

    // Cerramos la cola del cliente si está abierta
    if (client_queue != -1)
    {
        if (mq_close(client_queue) == -1)
        {
            sprintf(msgbuf, "Error al cerrar la cola del cliente: %s", strerror(errno));
            funcionLog(msgbuf, LOG_FILE);
        }
        else
        {
            funcionLog("Cola del cliente cerrada", LOG_FILE);
        }
    }

    // Eliminamos (unlink) la cola del servidor del sistema
    // mq_unlink elimina la cola del sistema, liberando todos los recursos asociados
    char server_queue_name[100];
    get_queue_name(server_queue_name, SERVER_QUEUE);
    if (mq_unlink(server_queue_name) == -1)
    {
        sprintf(msgbuf, "Error al eliminar la cola del servidor: %s", strerror(errno));
        funcionLog(msgbuf, LOG_FILE);
    }
    else
    {
        funcionLog("Cola del servidor eliminada", LOG_FILE);
    }

    // Eliminamos (unlink) la cola del cliente del sistema
    char client_queue_name[100];
    get_queue_name(client_queue_name, CLIENT_QUEUE);
    if (mq_unlink(client_queue_name) == -1)
    {
        sprintf(msgbuf, "Error al eliminar la cola del cliente: %s", strerror(errno));
        funcionLog(msgbuf, LOG_FILE);
    }
    else
    {
        funcionLog("Cola del cliente eliminada", LOG_FILE);
    }
}

/**
 * Función: handle_signal
 *
 * Manejador de señales para SIGINT (Ctrl+C) y SIGTERM.
 * Cuando se recibe una de estas señales, el servidor realiza una limpieza
 * ordenada de recursos y termina.
 *
 * Parámetros:
 *   - sig: Número de la señal recibida
 */
void handle_signal(int sig)
{
    char msgbuf[100];
    sprintf(msgbuf, "Recibida señal %d, terminando...", sig);
    funcionLog(msgbuf, LOG_FILE);

    // Llamamos a la función de limpieza para liberar recursos
    cleanup();

    // Terminamos el programa con éxito
    exit(EXIT_SUCCESS);
}

/**
 * Función: main
 *
 * Función principal del servidor. Crea las colas de mensajes, configura
 * los manejadores de señales y entra en un bucle para recibir mensajes
 * del cliente, contar caracteres y enviar respuestas.
 *
 * Retorno:
 *   - EXIT_SUCCESS si el programa termina correctamente
 *   - EXIT_FAILURE si ocurre algún error
 */
int main()
{
    char buffer[MAX_SIZE]; // Buffer para almacenar mensajes recibidos/enviados
    char msgbuf[MAX_SIZE]; // Buffer para mensajes de log
    unsigned int prio = 1; // Prioridad para recepción de mensajes

    // Configuramos los manejadores de señales
    // SIGINT (Ctrl+C) y SIGTERM son señales comunes para terminar procesos
    if (signal(SIGINT, handle_signal) == SIG_ERR)
    {
        funcionLog("Error al establecer el manejador de SIGINT", LOG_FILE);
        return EXIT_FAILURE;
    }

    if (signal(SIGTERM, handle_signal) == SIG_ERR)
    {
        funcionLog("Error al establecer el manejador de SIGTERM", LOG_FILE);
        return EXIT_FAILURE;
    }

    // Configuramos los atributos de las colas de mensajes
    struct mq_attr attr;
    attr.mq_flags = 0;          // 0 = cola bloqueante (por defecto)
    attr.mq_maxmsg = 10;        // Número máximo de mensajes en la cola
    attr.mq_msgsize = MAX_SIZE; // Tamaño máximo de cada mensaje
    attr.mq_curmsgs = 0;        // Número actual de mensajes (inicialmente 0)

    // Obtenemos nombres únicos para las colas basados en el nombre de usuario
    char server_queue_name[100];
    char client_queue_name[100];
    get_queue_name(server_queue_name, SERVER_QUEUE);
    get_queue_name(client_queue_name, CLIENT_QUEUE);

    // Registramos los nombres de las colas en el log
    sprintf(msgbuf, "El nombre de la cola del servidor es: %s", server_queue_name);
    funcionLog(msgbuf, LOG_FILE);
    sprintf(msgbuf, "El nombre de la cola del cliente es: %s", client_queue_name);
    funcionLog(msgbuf, LOG_FILE);

    // Creamos la cola del servidor
    // O_CREAT: Crea la cola si no existe
    // O_RDONLY: Abre la cola solo para lectura (el servidor lee mensajes del cliente)
    // 0644: Permisos de la cola (rw-r--r--)
    server_queue = mq_open(server_queue_name, O_CREAT | O_RDONLY, 0644, &attr);
    if (server_queue == -1)
    {
        sprintf(msgbuf, "Error al crear la cola del servidor: %s", strerror(errno));
        funcionLog(msgbuf, LOG_FILE);
        return EXIT_FAILURE;
    }

    sprintf(msgbuf, "El descriptor de la cola del servidor es: %d", server_queue);
    funcionLog(msgbuf, LOG_FILE);

    // Creamos la cola del cliente
    // O_CREAT: Crea la cola si no existe
    // O_WRONLY: Abre la cola solo para escritura (el servidor escribe respuestas al cliente)
    client_queue = mq_open(client_queue_name, O_CREAT | O_WRONLY, 0644, &attr);
    if (client_queue == -1)
    {
        sprintf(msgbuf, "Error al crear la cola del cliente: %s", strerror(errno));
        funcionLog(msgbuf, LOG_FILE);
        // Si hay error, cerramos y eliminamos la cola del servidor que ya habíamos creado
        mq_close(server_queue);
        mq_unlink(server_queue_name);
        return EXIT_FAILURE;
    }

    sprintf(msgbuf, "El descriptor de la cola del cliente es: %d", client_queue);
    funcionLog(msgbuf, LOG_FILE);

    // Bucle principal del servidor
    // El servidor se ejecuta indefinidamente hasta recibir un mensaje de salida o un error
    while (1)
    {
        // Recibimos un mensaje de la cola del servidor
        // mq_receive es una llamada bloqueante: el proceso se detendrá aquí hasta que
        // llegue un mensaje o se produzca un error
        // Parámetros:
        // - server_queue: Descriptor de la cola
        // - buffer: Buffer donde se almacenará el mensaje
        // - MAX_SIZE: Tamaño máximo del mensaje
        // - &prio: Puntero a variable donde se almacenará la prioridad del mensaje
        ssize_t bytes_read = mq_receive(server_queue, buffer, MAX_SIZE, &prio);

        // Verificamos si hubo error en la recepción
        if (bytes_read < 0)
        {
            sprintf(msgbuf, "Error al recibir mensaje: %s", strerror(errno));
            funcionLog(msgbuf, LOG_FILE);
            break; // Salimos del bucle en caso de error
        }

        // Aseguramos que el buffer termine con un carácter nulo
        // Esto es importante para funciones como strcmp y strlen
        buffer[bytes_read] = '\0';

        // Registramos el mensaje recibido en el log
        sprintf(msgbuf, "Recibido el mensaje: %s", buffer);
        funcionLog(msgbuf, LOG_FILE);

        // Verificamos si es un mensaje de salida
        if (strcmp(buffer, MSG_EXIT) == 0)
        {
            funcionLog("Recibido mensaje de salida, terminando...", LOG_FILE);
            break; // Salimos del bucle si recibimos el mensaje de salida
        }

        // Contamos el número de caracteres en el mensaje recibido
        // strlen devuelve la longitud de la cadena sin contar el carácter nulo
        int char_count = strlen(buffer);

        // Preparamos el mensaje de respuesta con el conteo de caracteres
        sprintf(buffer, "Número de caracteres recibidos: %d", char_count);

        // Registramos el mensaje de respuesta en el log
        sprintf(msgbuf, "Enviando respuesta: %s", buffer);
        funcionLog(msgbuf, LOG_FILE);

        // Enviamos la respuesta al cliente a través de la cola del cliente
        // Parámetros:
        // - client_queue: Descriptor de la cola
        // - buffer: Mensaje a enviar
        // - strlen(buffer) + 1: Longitud del mensaje (incluyendo el carácter nulo)
        // - 0: Prioridad del mensaje (0 es la más baja)
        if (mq_send(client_queue, buffer, strlen(buffer) + 1, 0) == -1)
        {
            sprintf(msgbuf, "Error al enviar respuesta: %s", strerror(errno));
            funcionLog(msgbuf, LOG_FILE);
            break; // Salimos del bucle en caso de error
        }
    }

    // Limpiamos los recursos antes de terminar
    // Esto incluye cerrar y eliminar las colas de mensajes
    cleanup();

    // Terminamos el programa con éxito
    return EXIT_SUCCESS;
}

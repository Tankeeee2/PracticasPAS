/**
 * Ejercicio 3: Cliente para comunicación mediante colas de mensajes POSIX
 *
 * Este programa implementa un cliente que envía cadenas de texto a un servidor,
 * el cual cuenta el número de caracteres y devuelve el resultado. La comunicación
 * se realiza mediante dos colas de mensajes POSIX:
 * - Una cola para enviar mensajes al servidor
 * - Otra cola para recibir respuestas del servidor
 *
 * El cliente asume que el servidor ya está en ejecución y ha creado las colas.
 */

#include "ej3_common.h" // Incluye definiciones y funciones comunes

/**
 * Nombre del archivo de log para el cliente
 *
 * Todas las acciones y eventos importantes se registrarán en este archivo
 * para facilitar la depuración y el seguimiento de la ejecución.
 */
#define LOG_FILE "log-cliente.txt"

/**
 * Variables globales para los descriptores de las colas de mensajes y control de ejecución
 *
 * Se definen como globales para que puedan ser accedidas desde la función
 * de limpieza y el manejador de señales.
 *
 * - server_queue: Descriptor de la cola para enviar mensajes al servidor
 * - client_queue: Descriptor de la cola para recibir respuestas del servidor
 * - running: Flag para controlar el bucle principal (1=ejecutando, 0=terminar)
 *
 * El valor inicial -1 indica que las colas no están abiertas.
 */
mqd_t server_queue = -1;
mqd_t client_queue = -1;
int running = 1;

/**
 * Función: cleanup
 *
 * Realiza la limpieza de recursos antes de terminar el programa.
 * Cierra las colas de mensajes si están abiertas.
 *
 * A diferencia del servidor, el cliente no elimina (unlink) las colas,
 * ya que esa responsabilidad corresponde al servidor.
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
}

/**
 * Función: handle_signal
 *
 * Manejador de señales para SIGINT (Ctrl+C) y SIGTERM.
 * Cuando se recibe una de estas señales, el cliente envía un mensaje de salida
 * al servidor, realiza una limpieza ordenada de recursos y termina.
 *
 * Parámetros:
 *   - sig: Número de la señal recibida
 */
void handle_signal(int sig)
{
    char msgbuf[100];
    // Registramos qué señal se recibió (SIGINT o SIGTERM)
    sprintf(msgbuf, "Recibida señal %d (%s), terminando...", sig, (sig == SIGINT) ? "SIGINT" : "SIGTERM");
    funcionLog(msgbuf, LOG_FILE);

    // Enviamos un mensaje de salida al servidor para que también termine
    if (server_queue != -1)
    {
        funcionLog("Enviando mensaje de salida al servidor", LOG_FILE);
        if (mq_send(server_queue, MSG_EXIT, strlen(MSG_EXIT) + 1, 0) == -1)
        {
            sprintf(msgbuf, "Error al enviar mensaje de salida: %s", strerror(errno));
            funcionLog(msgbuf, LOG_FILE);
        }
    }

    // Limpiamos los recursos
    cleanup();

    // Establecemos la variable running a 0 para salir del bucle principal
    // Esto es más elegante que llamar a exit() directamente, ya que permite
    // que el programa termine de forma ordenada
    running = 0;
}

/**
 * Función: main
 *
 * Función principal del cliente. Abre las colas de mensajes creadas por el servidor,
 * configura los manejadores de señales y entra en un bucle para enviar mensajes
 * al servidor y recibir respuestas.
 *
 * Retorno:
 *   - EXIT_SUCCESS si el programa termina correctamente
 *   - EXIT_FAILURE si ocurre algún error
 */
int main()
{
    char buffer[MAX_SIZE]; // Buffer para almacenar mensajes enviados/recibidos
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

    // Abrimos la cola del servidor para escritura
    // O_WRONLY: Abre la cola solo para escritura (el cliente escribe mensajes al servidor)
    // No usamos O_CREAT porque asumimos que el servidor ya ha creado la cola
    server_queue = mq_open(server_queue_name, O_WRONLY);
    if (server_queue == -1)
    {
        sprintf(msgbuf, "Error al abrir la cola del servidor: %s", strerror(errno));
        funcionLog(msgbuf, LOG_FILE);
        funcionLog("Asegúrese de que el servidor está en ejecución", LOG_FILE);
        return EXIT_FAILURE;
    }

    sprintf(msgbuf, "El descriptor de la cola del servidor es: %d", server_queue);
    funcionLog(msgbuf, LOG_FILE);

    // Abrimos la cola del cliente para lectura
    // O_RDONLY: Abre la cola solo para lectura (el cliente lee respuestas del servidor)
    client_queue = mq_open(client_queue_name, O_RDONLY);
    if (client_queue == -1)
    {
        sprintf(msgbuf, "Error al abrir la cola del cliente: %s", strerror(errno));
        funcionLog(msgbuf, LOG_FILE);
        // Si hay error, cerramos la cola del servidor que ya habíamos abierto
        mq_close(server_queue);
        return EXIT_FAILURE;
    }

    sprintf(msgbuf, "El descriptor de la cola del cliente es: %d", client_queue);
    funcionLog(msgbuf, LOG_FILE);

    // Mostramos instrucciones al usuario
    funcionLog("Mandando mensajes al servidor (escribir \"exit\" para parar):", LOG_FILE);

    // Bucle principal del cliente
    // El cliente se ejecuta hasta que el usuario escriba "exit" o se reciba una señal
    while (running)
    {
        // Mostramos un prompt para que el usuario sepa que puede escribir
        printf("> ");
        fflush(stdout); // Forzamos la salida del buffer para que se muestre el prompt

        // Leemos una línea de la entrada estándar (teclado)
        if (fgets(buffer, MAX_SIZE, stdin) == NULL)
        {
            // Verificamos si llegamos al final de la entrada (Ctrl+D)
            if (feof(stdin))
            {
                funcionLog("Fin de entrada estándar, terminando...", LOG_FILE);
                break;
            }
            else
            {
                // Otro tipo de error al leer
                sprintf(msgbuf, "Error al leer de stdin: %s", strerror(errno));
                funcionLog(msgbuf, LOG_FILE);
                break;
            }
        }

        // Eliminamos el carácter de nueva línea (\n) del final si existe
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n')
        {
            buffer[len - 1] = '\0'; // Reemplazamos \n por \0 (fin de cadena)
            len--;                  // Actualizamos la longitud
        }

        // Si la línea está vacía, volvemos al inicio del bucle
        if (len == 0)
        {
            continue;
        }

        // Verificamos si el usuario quiere salir
        if (strcmp(buffer, MSG_EXIT) == 0)
        {
            funcionLog("Enviando mensaje de salida, terminando...", LOG_FILE);
            // Enviamos el mensaje de salida al servidor
            if (mq_send(server_queue, buffer, strlen(buffer) + 1, 0) == -1)
            {
                sprintf(msgbuf, "Error al enviar mensaje: %s", strerror(errno));
                funcionLog(msgbuf, LOG_FILE);
            }
            break; // Salimos del bucle
        }

        // Registramos el mensaje que vamos a enviar
        sprintf(msgbuf, "Enviando mensaje: %s", buffer);
        funcionLog(msgbuf, LOG_FILE);

        // Enviamos el mensaje al servidor
        // Parámetros:
        // - server_queue: Descriptor de la cola
        // - buffer: Mensaje a enviar
        // - strlen(buffer) + 1: Longitud del mensaje (incluyendo el carácter nulo)
        // - 0: Prioridad del mensaje (0 es la más baja)
        if (mq_send(server_queue, buffer, strlen(buffer) + 1, 0) == -1)
        {
            sprintf(msgbuf, "Error al enviar mensaje: %s", strerror(errno));
            funcionLog(msgbuf, LOG_FILE);
            break; // Salimos del bucle en caso de error
        }

        // Recibimos la respuesta del servidor
        // mq_receive es una llamada bloqueante: el proceso se detendrá aquí hasta que
        // llegue un mensaje o se produzca un error
        ssize_t bytes_read = mq_receive(client_queue, buffer, MAX_SIZE, &prio);

        // Verificamos si hubo error en la recepción
        if (bytes_read < 0)
        {
            sprintf(msgbuf, "Error al recibir respuesta: %s", strerror(errno));
            funcionLog(msgbuf, LOG_FILE);
            break; // Salimos del bucle en caso de error
        }

        // Aseguramos que el buffer termine con un carácter nulo
        buffer[bytes_read] = '\0';

        // Registramos la respuesta recibida
        sprintf(msgbuf, "Respuesta del servidor: %s", buffer);
        funcionLog(msgbuf, LOG_FILE);
    }

    // Limpiamos los recursos antes de terminar
    cleanup();

    // Terminamos el programa con éxito
    return EXIT_SUCCESS;
}

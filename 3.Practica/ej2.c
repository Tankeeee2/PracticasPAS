/**
 * Ejercicio 2: Programa que utiliza tuberías (pipes) para comunicación entre procesos
 *
 * Este programa crea dos procesos (padre e hijo) que se comunican mediante una tubería.
 * El proceso padre genera dos números aleatorios flotantes, calcula su suma y envía
 * el resultado al proceso hijo a través de la tubería. El proceso hijo recibe la suma
 * y la muestra por pantalla.
 *
 * Este ejercicio demuestra el uso de:
 * - Tuberías (pipes) para comunicación unidireccional entre procesos
 * - Creación de procesos con fork()
 * - Generación de números aleatorios
 * - Sincronización entre procesos padre e hijo
 */

#include <errno.h>    // Para códigos de error (errno) y funciones relacionadas
#include <stdio.h>    // Para funciones de entrada/salida estándar
#include <stdlib.h>   // Para funciones como exit(), rand(), srand()
#include <string.h>   // Para funciones de manejo de cadenas como strlen()
#include <sys/wait.h> // Para la función wait() que espera a que termine un proceso hijo
#include <time.h>     // Para time(), usado para inicializar la semilla aleatoria
#include <unistd.h>   // Para funciones POSIX como pipe(), fork(), read(), write()

int main()
{
    pid_t flag;            // Almacena el PID del proceso hijo retornado por wait()
    int status;            // Almacena el estado de salida del proceso hijo
    int fildes[2];         // Array para los descriptores de la tubería [0]=lectura, [1]=escritura
    const int BSIZE = 100; // Tamaño del buffer para la comunicación
    char buf[BSIZE];       // Buffer para almacenar los datos a transmitir
    ssize_t nbytes;        // Número de bytes leídos de la tubería
    float num1, num2, sum; // Variables para los números aleatorios y su suma

    // Inicializamos la semilla para la generación de números aleatorios
    // Esto garantiza que obtengamos números diferentes en cada ejecución
    srand(time(NULL));

    // Creamos la tubería (pipe)
    // pipe() crea un par de descriptores de archivo en fildes:
    // - fildes[0] es el extremo de lectura
    // - fildes[1] es el extremo de escritura
    status = pipe(fildes);
    if (status == -1) {
        // Si hay un error al crear la tubería, mostramos el mensaje y terminamos
        perror("Error en pipe");
        exit(EXIT_FAILURE);
    }

    // Creamos un nuevo proceso con fork()
    // fork() devuelve:
    // - -1 en caso de error
    // - 0 en el proceso hijo
    // - PID del hijo en el proceso padre
    switch (fork()) {
    case -1: // Error al crear el proceso hijo
        perror("No se ha podido crear el proceso hijo...");
        exit(EXIT_FAILURE);

    case 0: // Código ejecutado por el proceso hijo
        // El hijo solo necesita leer de la tubería, así que cerramos el extremo de escritura
        // Esto es una buena práctica para liberar recursos y evitar bloqueos
        if (close(fildes[1]) == -1) {
            perror("Error en close");
            exit(EXIT_FAILURE);
        }

        // Leemos datos de la tubería
        // read() es una llamada bloqueante: el proceso se detendrá aquí hasta que haya datos
        // disponibles o hasta que se cierre el otro extremo de la tubería
        nbytes = read(fildes[0], buf, BSIZE);

        if (nbytes == -1) {
            // Error al leer de la tubería
            perror("Error en read");
            exit(EXIT_FAILURE);
        }
        else if (nbytes == 0) {
            // Si read() devuelve 0, significa que el otro extremo de la tubería se ha cerrado
            // y no hay más datos para leer (EOF - End Of File)
            printf("[HIJO]: Detecto que mi padre ha cerrado la tuberia...\n");
        }
        else {
            // Convertimos la cadena recibida a un número flotante
            float result;
            sscanf(buf, "%f", &result);
            printf("[HIJO]: He recibido la suma: %f\n", result);
        }

        // Cerramos el extremo de lectura de la tubería cuando ya no lo necesitamos
        if (close(fildes[0]) == -1) {
            perror("Error en close");
            exit(EXIT_FAILURE);
        }
        else {
            printf("[HIJO]: Tuberia cerrada.\n");
        }

        // Terminamos el proceso hijo con éxito
        exit(EXIT_SUCCESS);

    default: // Código ejecutado por el proceso padre
        // El padre solo necesita escribir en la tubería, así que cerramos el extremo de lectura
        if (close(fildes[0]) == -1) {
            perror("Error en close");
            exit(EXIT_FAILURE);
        }

        // Generamos dos números aleatorios flotantes entre 0 y 100
        // rand() devuelve un entero entre 0 y RAND_MAX
        // Dividimos por RAND_MAX para obtener un valor entre 0 y 1
        // Multiplicamos por 100 para obtener un valor entre 0 y 100
        num1 = (float)rand() / RAND_MAX * 100.0;
        num2 = (float)rand() / RAND_MAX * 100.0;

        // Calculamos la suma de los dos números
        sum = num1 + num2;

        // Mostramos los números generados y su suma
        printf("[PADRE]: Generados dos números aleatorios: %f y %f\n", num1, num2);
        printf("[PADRE]: La suma es: %f\n", sum);

        // Convertimos la suma a una cadena y la escribimos en la tubería
        // sprintf() convierte el número a cadena y lo almacena en buf
        sprintf(buf, "%f", sum);

        // write() escribe la cadena en la tubería
        // strlen(buf) + 1 incluye el carácter nulo de terminación
        if (write(fildes[1], buf, strlen(buf) + 1) == -1) {
            perror("Error en write");
            exit(EXIT_FAILURE);
        }

        // Cerramos el extremo de escritura de la tubería cuando ya no lo necesitamos
        // Esto es importante para que el proceso hijo pueda detectar el EOF
        if (close(fildes[1]) == -1) {
            perror("Error en close");
            exit(EXIT_FAILURE);
        }
        else {
            printf("[PADRE]: Tuberia cerrada.\n");
        }

        // Esperamos a que el proceso hijo termine
        // wait() bloquea al proceso padre hasta que un hijo termine
        // Recorremos todos los hijos (en este caso solo hay uno)
        while ((flag = wait(&status)) > 0) {
            if (WIFEXITED(status)) {
                // WIFEXITED comprueba si el hijo terminó normalmente (con exit)
                // WEXITSTATUS obtiene el código de salida del hijo
                printf("Proceso Padre, Hijo con PID %ld finalizado, status = %d\n", (long int)flag,
                       WEXITSTATUS(status));
            }
            else if (WIFSIGNALED(status)) {
                // WIFSIGNALED comprueba si el hijo terminó debido a una señal
                // WTERMSIG obtiene el número de la señal que causó la terminación
                printf("Proceso Padre, Hijo con PID %ld finalizado al recibir la señal %d\n",
                       (long int)flag, WTERMSIG(status));
            }
        }

        // Comprobamos si wait() terminó porque no hay más hijos
        if (flag == (pid_t)-1 && errno == ECHILD) {
            // ECHILD indica que no hay procesos hijos que esperar
            printf("Proceso Padre %d, no hay mas hijos que esperar. Valor de errno = %d, definido "
                   "como: %s\n",
                   getpid(), errno, strerror(errno));
        }
        else {
            // Cualquier otro error en wait()
            printf("Error en la invocacion de wait o waitpid. Valor de errno = %d, definido como: "
                   "%s\n",
                   errno, strerror(errno));
            exit(EXIT_FAILURE);
        }

        // Terminamos el proceso padre con éxito
        exit(EXIT_SUCCESS);
    }
}

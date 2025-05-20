/**
 * Ejercicio 1: Programa para obtener información de usuarios y grupos del sistema
 *
 * Este programa utiliza las funciones POSIX para acceder a la información de usuarios
 * y grupos del sistema operativo. Permite consultar información detallada sobre
 * usuarios específicos, el usuario activo, grupos específicos o todos los grupos
 * del sistema, según las opciones proporcionadas por línea de comandos.
 */

#include <ctype.h>  // Para funciones de clasificación de caracteres (isdigit)
#include <getopt.h> // Para procesar opciones de línea de comandos (getopt_long)
#include <grp.h>    // Para acceder a la información de grupos (struct group)
#include <pwd.h>    // Para acceder a la información de usuarios (struct passwd)
#include <stdio.h>  // Para funciones de entrada/salida estándar
#include <stdlib.h> // Para funciones como exit(), atoi(), getenv()
#include <string.h> // Para funciones de manejo de cadenas
#include <unistd.h> // Para funciones POSIX básicas

/**
 * Función: print_help
 *
 * Muestra un mensaje de ayuda con todas las opciones disponibles del programa.
 * Esta función se llama cuando se usa la opción -h/--help o cuando hay un error
 * en los parámetros proporcionados.
 */
void print_help()
{
    printf("Uso del programa: ejercicio1 [opciones]\n");
    printf("Opciones:\n");
    printf("-h, --help                      Imprimir esta ayuda\n");
    printf("-u, --user (<nombre>|<uid>)     Información sobre el usuario\n");
    printf("-a, --active                    Información sobre el usuario activo actual\n");
    printf("-m, --maingroup                 Además de info de usuario, imprimir la info de su "
           "grupo principal\n");
    printf("-g, --group (<nombre>|<gid>)    Información sobre el grupo\n");
    printf("-s, --allgroups                 Muestra info de todos los grupos del sistema\n");
}

/**
 * Función: print_user_info
 *
 * Muestra toda la información disponible de un usuario a partir de su estructura passwd.
 *
 * Parámetros:
 *   - pwd: Puntero a una estructura passwd que contiene la información del usuario.
 *          Esta estructura se obtiene mediante las funciones getpwnam() o getpwuid().
 *
 * La estructura passwd contiene los siguientes campos principales:
 *   - pw_name: Nombre de usuario (login)
 *   - pw_passwd: Contraseña encriptada (normalmente "*" por seguridad)
 *   - pw_uid: ID de usuario (UID)
 *   - pw_gid: ID del grupo principal
 *   - pw_gecos: Nombre completo y otra información del usuario
 *   - pw_dir: Directorio home del usuario
 *   - pw_shell: Shell por defecto del usuario
 */
void print_user_info(struct passwd *pwd)
{
    // Verificamos que la estructura no sea NULL
    if (pwd == NULL) {
        printf("Error: Usuario no encontrado\n");
        return;
    }

    // Mostramos toda la información del usuario
    printf("Usuario:\n");
    printf("Nombre: %s\n", pwd->pw_gecos);
    printf("Login: %s\n", pwd->pw_name);
    printf("Password: %s\n", pwd->pw_passwd);
    printf("UID: %d\n", pwd->pw_uid);
    printf("Home: %s\n", pwd->pw_dir);
    printf("Shell: %s\n", pwd->pw_shell);
    printf("Número de grupo principal: %d\n", pwd->pw_gid);
}

/**
 * Función: print_group_info
 *
 * Muestra toda la información disponible de un grupo a partir de su estructura group.
 *
 * Parámetros:
 *   - grp: Puntero a una estructura group que contiene la información del grupo.
 *          Esta estructura se obtiene mediante las funciones getgrnam() o getgrgid().
 *
 * La estructura group contiene los siguientes campos principales:
 *   - gr_name: Nombre del grupo
 *   - gr_gid: ID del grupo (GID)
 *   - gr_mem: Array de punteros a cadenas con los nombres de los miembros secundarios
 */
void print_group_info(struct group *grp)
{
    // Verificamos que la estructura no sea NULL
    if (grp == NULL) {
        printf("Error: Grupo no encontrado\n");
        return;
    }

    // Mostramos la información básica del grupo
    printf("Grupo:\n");
    printf("Nombre del grupo: %s\n", grp->gr_name);
    printf("GID: %d\n", grp->gr_gid);
    printf("Miembros secundarios:\n");

    // Mostramos los miembros secundarios del grupo si existen
    if (grp->gr_mem != NULL) {
        // Recorremos el array de miembros hasta encontrar un NULL
        for (int i = 0; grp->gr_mem[i] != NULL; i++) {
            printf("%s\n", grp->gr_mem[i]);
        }
    }
}

/**
 * Función: is_number
 *
 * Verifica si una cadena contiene solo dígitos (es un número).
 *
 * Parámetros:
 *   - str: Cadena a verificar
 *
 * Retorno:
 *   - 1 (true) si la cadena contiene solo dígitos
 *   - 0 (false) si la cadena es NULL, vacía o contiene caracteres no numéricos
 *
 * Esta función se utiliza para determinar si un argumento debe interpretarse
 * como un UID/GID numérico o como un nombre de usuario/grupo.
 */
int is_number(const char *str)
{
    // Verificamos que la cadena no sea NULL ni vacía
    if (str == NULL || *str == '\0') {
        return 0;
    }

    // Recorremos cada carácter de la cadena
    for (int i = 0; str[i] != '\0'; i++) {
        // Si encontramos un carácter que no es un dígito, retornamos falso
        if (!isdigit(str[i])) {
            return 0;
        }
    }

    // Si todos los caracteres son dígitos, retornamos verdadero
    return 1;
}

/**
 * Función: print_all_groups
 *
 * Lee el archivo /etc/group que contiene la información de todos los grupos
 * del sistema y muestra la información detallada de cada uno.
 *
 * En sistemas POSIX, el archivo /etc/group contiene la información de los grupos
 * en formato de texto, con campos separados por dos puntos (:).
 * Formato: nombre_grupo:contraseña:GID:lista_miembros
 */
void print_all_groups()
{
    // Abrimos el archivo /etc/group en modo lectura
    FILE *file = fopen("/etc/group", "r");
    if (file == NULL) {
        perror("Error al abrir /etc/group");
        return;
    }

    // Buffers para leer líneas y extraer nombres de grupos
    char line[256];
    char group_name[64];

    // Leemos el archivo línea por línea
    while (fgets(line, sizeof(line), file)) {
        // Extraemos el nombre del grupo (primer campo antes del ':')
        sscanf(line, "%[^:]", group_name);

        // Obtenemos la información completa del grupo usando su nombre
        struct group *grp = getgrnam(group_name);
        if (grp != NULL) {
            // Mostramos la información del grupo
            print_group_info(grp);
        }
    }

    // Cerramos el archivo
    fclose(file);
}

/**
 * Función: main
 *
 * Función principal del programa. Procesa los argumentos de línea de comandos
 * y ejecuta las acciones correspondientes según las opciones proporcionadas.
 *
 * Parámetros:
 *   - argc: Número de argumentos de línea de comandos
 *   - argv: Array de cadenas con los argumentos
 *
 * Retorno:
 *   - 0 si el programa se ejecuta correctamente
 *   - 1 si hay algún error
 */
int main(int argc, char *argv[])
{
    // Variables para el procesamiento de opciones
    int opt;              // Almacena la opción actual procesada por getopt_long
    int option_index = 0; // Índice de la opción larga actual

    // Flags para las diferentes opciones
    int user_flag = 0;      // Indica si se especificó la opción -u/--user
    int active_flag = 0;    // Indica si se especificó la opción -a/--active
    int maingroup_flag = 0; // Indica si se especificó la opción -m/--maingroup
    int group_flag = 0;     // Indica si se especificó la opción -g/--group
    int allgroups_flag = 0; // Indica si se especificó la opción -s/--allgroups

    // Argumentos para las opciones que los requieren
    char *user_arg = NULL;  // Argumento para -u/--user (nombre o UID)
    char *group_arg = NULL; // Argumento para -g/--group (nombre o GID)

    // Definición de las opciones largas para getopt_long
    // Formato: {nombre_largo, tiene_argumento, flag, valor_retorno}
    // - nombre_largo: Nombre de la opción larga (sin --)
    // - tiene_argumento: no_argument (0), required_argument (1), optional_argument (2)
    // - flag: Si es NULL, getopt_long retorna valor_retorno; si no, *flag = valor_retorno
    // - valor_retorno: Valor a retornar o asignar a *flag
    static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                           {"user", required_argument, 0, 'u'},
                                           {"active", no_argument, 0, 'a'},
                                           {"maingroup", no_argument, 0, 'm'},
                                           {"group", required_argument, 0, 'g'},
                                           {"allgroups", no_argument, 0, 's'},
                                           {0, 0, 0, 0}}; // El último elemento debe ser {0,0,0,0}

    // Procesamos las opciones de línea de comandos
    // getopt_long busca opciones que empiecen con - o --
    // "hu:amg:s" especifica las opciones cortas: h,u:,a,m,g:,s (: indica que requiere argumento)
    while ((opt = getopt_long(argc, argv, "hu:amg:s", long_options, &option_index)) != -1) {
        switch (opt) {
        case 'h': // Opción -h/--help
            print_help();
            return 0;
        case 'u': // Opción -u/--user
            user_flag = 1;
            user_arg = optarg; // optarg contiene el argumento de la opción
            break;
        case 'a': // Opción -a/--active
            active_flag = 1;
            break;
        case 'm': // Opción -m/--maingroup
            maingroup_flag = 1;
            break;
        case 'g': // Opción -g/--group
            group_flag = 1;
            group_arg = optarg;
            break;
        case 's': // Opción -s/--allgroups
            allgroups_flag = 1;
            break;
        default: // Opción no reconocida
            print_help();
            return 1;
        }
    }

    // Verificamos combinaciones inválidas de opciones
    if ((user_flag && active_flag) ||     // No se puede especificar --user y --active
        (group_flag && allgroups_flag) || // No se puede especificar --group y --allgroups
        (maingroup_flag && !(user_flag || active_flag)) || // --maingroup requiere --user o --active
        (allgroups_flag && maingroup_flag)) // No se puede combinar --allgroups con --maingroup
    {
        // Mostramos un mensaje de error específico según el caso
        if (maingroup_flag && !(user_flag || active_flag)) {
            printf("La opción --maingroup sólo puede acompañar a --user o --active\n");
        }
        else if (allgroups_flag && maingroup_flag) {
            printf("La opción --allgroups no puede combinarse con --maingroup\n");
        }
        else {
            printf("Combinación de opciones no válida\n");
        }

        print_help();
        return 1;
    }

    // Si no se especificaron opciones, mostramos información del usuario actual y su grupo
    // principal
    if (argc == 1) {
        active_flag = 1;
        maingroup_flag = 1;
    }

    // Variable para almacenar la información del usuario
    struct passwd *pwd = NULL;

    // Procesamos la opción --user si se especificó
    if (user_flag) {
        // Determinamos si el argumento es un UID (número) o un nombre de usuario
        if (is_number(user_arg)) {
            // Convertimos el argumento a un UID y obtenemos la información del usuario
            uid_t uid = atoi(user_arg);
            pwd = getpwuid(uid); // getpwuid busca un usuario por su UID
        }
        else {
            // Obtenemos la información del usuario por su nombre
            pwd = getpwnam(user_arg); // getpwnam busca un usuario por su nombre
        }

        // Verificamos si se encontró el usuario
        if (pwd == NULL) {
            printf("Error: Usuario '%s' no encontrado\n", user_arg);
            return 1;
        }

        // Mostramos la información del usuario
        print_user_info(pwd);
    }

    // Procesamos la opción --active si se especificó
    if (active_flag) {
        // Obtenemos el nombre del usuario actual de la variable de entorno USER
        char *username = getenv("USER");
        if (username == NULL) {
            printf("Error: No se pudo obtener el usuario activo\n");
            return 1;
        }

        // Obtenemos la información del usuario actual
        pwd = getpwnam(username);
        if (pwd == NULL) {
            printf("Error: Usuario activo no encontrado\n");
            return 1;
        }

        // Mostramos la información del usuario actual
        print_user_info(pwd);
    }

    // Procesamos la opción --maingroup si se especificó y tenemos información de usuario
    if (maingroup_flag && pwd != NULL) {
        // Obtenemos la información del grupo principal del usuario
        struct group *grp = getgrgid(pwd->pw_gid); // getgrgid busca un grupo por su GID
        if (grp == NULL) {
            printf("Error: Grupo principal del usuario no encontrado\n");
            return 1;
        }

        // Mostramos la información del grupo principal
        print_group_info(grp);
    }

    // Procesamos la opción --group si se especificó
    if (group_flag) {
        struct group *grp = NULL;

        // Determinamos si el argumento es un GID (número) o un nombre de grupo
        if (is_number(group_arg)) {
            // Convertimos el argumento a un GID y obtenemos la información del grupo
            gid_t gid = atoi(group_arg);
            grp = getgrgid(gid);
        }
        else {
            // Obtenemos la información del grupo por su nombre
            grp = getgrnam(group_arg); // getgrnam busca un grupo por su nombre
        }

        // Verificamos si se encontró el grupo
        if (grp == NULL) {
            printf("Error: Grupo '%s' no encontrado\n", group_arg);
            return 1;
        }

        // Mostramos la información del grupo
        print_group_info(grp);
    }

    // Procesamos la opción --allgroups si se especificó
    if (allgroups_flag) {
        // Mostramos la información de todos los grupos del sistema
        print_all_groups();
    }

    return 0;
}
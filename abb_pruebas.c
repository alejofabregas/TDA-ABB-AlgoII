#define _POSIX_C_SOURCE 200809L

#include "abb.h"
#include "testing.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SUMA_ELEM 0
#define CANT_ITERACIONES 1
#define PALABRA 0
#define PALABRA_ARMADA 1


/* *****************************************************************
 *                    FUNCIONES AUXILIARES
 * *****************************************************************/

// Fuente: https://stackoverflow.com/questions/42321370/fisher-yates-shuffling-algorithm-in-c
// Mas info: https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
void FisherYates(char** claves, size_t largo) { // implementation of Fisher Yates
    size_t i, j;
    char* aux[largo];

     for (i = largo - 1; i > 0; i--) { // for loop to shuffle
         j = rand()%(largo-1); // randomise j for shuffle with Fisher Yates
         aux[i] = claves[j];
         claves[j] = claves[i];
         claves[i] = aux[i];
     }
}

static ssize_t buscar(const char* clave, char* claves[], size_t largo) {
    for (size_t i = 0; i < largo; i++) {
        if (strcmp(clave, claves[i]) == 0) return (ssize_t) i;
    }
    return -1;
}

// Para el iterador interno:

bool sumar_elementos(const char* clave, void* dato, void* extra) {
	*(int*)extra += *(int*)dato;
	return true;
}

bool suma_primeros_3(const char* clave, void* dato, void* extra) {
	int* suma_y_cant_iter = (int*)extra;
	suma_y_cant_iter[SUMA_ELEM] += *(int*)dato;
	suma_y_cant_iter[CANT_ITERACIONES] += 1;
	return suma_y_cant_iter[CANT_ITERACIONES] < 3 ? true : false;
}

bool encontrar_palabra(const char* clave, void* dato, void* extra) {
	char* caracter_iterado = (char*)dato;
	char** palabras = (char**)extra;
	strcat(palabras[PALABRA_ARMADA], caracter_iterado);
	if (strcmp(palabras[PALABRA_ARMADA], palabras[PALABRA]) == 0) return false;
	return true;
}

typedef struct suma_y_clave {
	int suma;
	char* clave;
} suma_y_clave_t;

bool sumar_hasta_clave(const char* clave, void* dato, void* extra) {
	suma_y_clave_t* suma_y_clave = (suma_y_clave_t*)extra;
	suma_y_clave->suma += *(int*)dato;
	return strcmp(suma_y_clave->clave,clave) == 0 ? false : true;
}


/* ******************************************************************
 *                        PRUEBAS UNITARIAS
 * *****************************************************************/

static void prueba_crear_abb_vacio() {
	printf("\n--PRUEBAS CON ABB VACIO--\n");
    abb_t* abb = abb_crear(strcmp, NULL);

    print_test("Prueba abb crear abb vacio", abb);
    print_test("Prueba abb la cantidad de elementos es 0", abb_cantidad(abb) == 0);
    print_test("Prueba abb obtener clave A, es NULL, no existe", !abb_obtener(abb, "A"));
    print_test("Prueba abb pertenece clave A, es false, no existe", !abb_pertenece(abb, "A"));
    print_test("Prueba abb borrar clave A, es NULL, no existe", !abb_borrar(abb, "A"));

    abb_destruir(abb);
}

static void prueba_abb_insertar() {
	printf("\n--PRUEBAS CON ABB INSERTAR--\n");
    abb_t* abb = abb_crear(strcmp, NULL);

    char *clave1 = "perro", *valor1 = "guau";
    char *clave2 = "gato", *valor2 = "miau";
    char *clave3 = "vaca", *valor3 = "mu";

    /* Inserta 1 valor y luego lo borra */
    print_test("Prueba abb insertar clave1", abb_guardar(abb, clave1, valor1));
    print_test("Prueba abb la cantidad de elementos es 1", abb_cantidad(abb) == 1);
    print_test("Prueba abb obtener clave1 es valor1", abb_obtener(abb, clave1) == valor1);
    print_test("Prueba abb obtener clave1 es valor1", abb_obtener(abb, clave1) == valor1);
    print_test("Prueba abb pertenece clave1, es true", abb_pertenece(abb, clave1));
    print_test("Prueba abb borrar clave1, es valor1", abb_borrar(abb, clave1) == valor1);
    print_test("Prueba abb la cantidad de elementos es 0", abb_cantidad(abb) == 0);

    /* Inserta otros 2 valores y no los borra (se destruyen con el abb) */
    print_test("Prueba abb insertar clave2", abb_guardar(abb, clave2, valor2));
    print_test("Prueba abb la cantidad de elementos es 1", abb_cantidad(abb) == 1);
    print_test("Prueba abb obtener clave2 es valor2", abb_obtener(abb, clave2) == valor2);
    print_test("Prueba abb obtener clave2 es valor2", abb_obtener(abb, clave2) == valor2);
    print_test("Prueba abb pertenece clave2, es true", abb_pertenece(abb, clave2));

    print_test("Prueba abb insertar clave3", abb_guardar(abb, clave3, valor3));
    print_test("Prueba abb la cantidad de elementos es 2", abb_cantidad(abb) == 2);
    print_test("Prueba abb obtener clave3 es valor3", abb_obtener(abb, clave3) == valor3);
    print_test("Prueba abb obtener clave3 es valor3", abb_obtener(abb, clave3) == valor3);
    print_test("Prueba abb pertenece clave3, es true", abb_pertenece(abb, clave3));

    abb_destruir(abb);
}

static void prueba_abb_reemplazar() {
	printf("\n--PRUEBAS REEMPLAZANDO EN EL ABB--\n");
    abb_t* abb = abb_crear(strcmp, NULL);
    char *clave1 = "perro", *valor1a = "guau", *valor1b = "warf";
    char *clave2 = "gato", *valor2a = "miau", *valor2b = "meaow";

    /* Inserta 2 valores y luego los reemplaza */
    print_test("Prueba abb insertar clave1", abb_guardar(abb, clave1, valor1a));
    print_test("Prueba abb obtener clave1 es valor1a", abb_obtener(abb, clave1) == valor1a);
    print_test("Prueba abb obtener clave1 es valor1a", abb_obtener(abb, clave1) == valor1a);
    print_test("Prueba abb insertar clave2", abb_guardar(abb, clave2, valor2a));
    print_test("Prueba abb obtener clave2 es valor2a", abb_obtener(abb, clave2) == valor2a);
    print_test("Prueba abb obtener clave2 es valor2a", abb_obtener(abb, clave2) == valor2a);
    print_test("Prueba abb la cantidad de elementos es 2", abb_cantidad(abb) == 2);

    print_test("Prueba abb insertar clave1 con otro valor", abb_guardar(abb, clave1, valor1b));
    print_test("Prueba abb obtener clave1 es valor1b", abb_obtener(abb, clave1) == valor1b);
    print_test("Prueba abb obtener clave1 es valor1b", abb_obtener(abb, clave1) == valor1b);
    print_test("Prueba abb insertar clave2 con otro valor", abb_guardar(abb, clave2, valor2b));
    print_test("Prueba abb obtener clave2 es valor2b", abb_obtener(abb, clave2) == valor2b);
    print_test("Prueba abb obtener clave2 es valor2b", abb_obtener(abb, clave2) == valor2b);
    print_test("Prueba abb la cantidad de elementos es 2", abb_cantidad(abb) == 2);

    abb_destruir(abb);
}

static void prueba_abb_reemplazar_con_destruir() {
	printf("\n--PRUEBAS REEMPLAZANDO EN EL ABB CON FUNCION DESTRUIR--\n");
    abb_t* abb = abb_crear(strcmp, free);

    char *clave1 = "perro", *valor1a, *valor1b;
    char *clave2 = "gato", *valor2a, *valor2b;

    // Pide memoria para 4 valores 
    valor1a = malloc(10 * sizeof(char));
    valor1b = malloc(10 * sizeof(char));
    valor2a = malloc(10 * sizeof(char));
    valor2b = malloc(10 * sizeof(char));

    // Inserta 2 valores y luego los reemplaza (debe liberar lo que reemplaza) 
    print_test("Prueba abb insertar clave1", abb_guardar(abb, clave1, valor1a));
    print_test("Prueba abb obtener clave1 es valor1a", abb_obtener(abb, clave1) == valor1a);
    print_test("Prueba abb obtener clave1 es valor1a", abb_obtener(abb, clave1) == valor1a);
    print_test("Prueba abb insertar clave2", abb_guardar(abb, clave2, valor2a));
    print_test("Prueba abb obtener clave2 es valor2a", abb_obtener(abb, clave2) == valor2a);
    print_test("Prueba abb obtener clave2 es valor2a", abb_obtener(abb, clave2) == valor2a);
    print_test("Prueba abb la cantidad de elementos es 2", abb_cantidad(abb) == 2);

    print_test("Prueba abb insertar clave1 con otro valor", abb_guardar(abb, clave1, valor1b));
    print_test("Prueba abb obtener clave1 es valor1b", abb_obtener(abb, clave1) == valor1b);
    print_test("Prueba abb obtener clave1 es valor1b", abb_obtener(abb, clave1) == valor1b);
    print_test("Prueba abb insertar clave2 con otro valor", abb_guardar(abb, clave2, valor2b));
    print_test("Prueba abb obtener clave2 es valor2b", abb_obtener(abb, clave2) == valor2b);
    print_test("Prueba abb obtener clave2 es valor2b", abb_obtener(abb, clave2) == valor2b);
    print_test("Prueba abb la cantidad de elementos es 2", abb_cantidad(abb) == 2);

    //Se destruye el abb (se debe liberar lo que quedó dentro) 
    abb_destruir(abb);
}

static void prueba_abb_borrar() {
	printf("\n--PRUEBAS CON ABB BORRAR--\n");
    abb_t* abb = abb_crear(strcmp, NULL);

    char *clave1 = "perro", *valor1 = "guau";
    char *clave2 = "gato", *valor2 = "miau";
    char *clave3 = "vaca", *valor3 = "mu";

    /* Inserta 3 valores y luego los borra */
    print_test("Prueba abb insertar clave1", abb_guardar(abb, clave1, valor1));
    print_test("Prueba abb insertar clave2", abb_guardar(abb, clave2, valor2));
    print_test("Prueba abb insertar clave3", abb_guardar(abb, clave3, valor3));

    /* Al borrar cada elemento comprueba que ya no está pero los otros sí. */
    print_test("Prueba abb pertenece clave3, es verdadero", abb_pertenece(abb, clave3));
    print_test("Prueba abb borrar clave3, es valor3", abb_borrar(abb, clave3) == valor3);
    print_test("Prueba abb borrar clave3, es NULL", !abb_borrar(abb, clave3));
    print_test("Prueba abb pertenece clave3, es falso", !abb_pertenece(abb, clave3));
    print_test("Prueba abb obtener clave3, es NULL", !abb_obtener(abb, clave3));
    print_test("Prueba abb la cantidad de elementos es 2", abb_cantidad(abb) == 2);

    print_test("Prueba abb pertenece clave1, es verdadero", abb_pertenece(abb, clave1));
    print_test("Prueba abb borrar clave1, es valor1", abb_borrar(abb, clave1) == valor1);
    print_test("Prueba abb borrar clave1, es NULL", !abb_borrar(abb, clave1));
    print_test("Prueba abb pertenece clave1, es falso", !abb_pertenece(abb, clave1));
    print_test("Prueba abb obtener clave1, es NULL", !abb_obtener(abb, clave1));
    print_test("Prueba abb la cantidad de elementos es 1", abb_cantidad(abb) == 1);

    print_test("Prueba abb pertenece clave2, es verdadero", abb_pertenece(abb, clave2));
    print_test("Prueba abb borrar clave2, es valor2", abb_borrar(abb, clave2) == valor2);
    print_test("Prueba abb borrar clave2, es NULL", !abb_borrar(abb, clave2));
    print_test("Prueba abb pertenece clave2, es falso", !abb_pertenece(abb, clave2));
    print_test("Prueba abb obtener clave2, es NULL", !abb_obtener(abb, clave2));
    print_test("Prueba abb la cantidad de elementos es 0", abb_cantidad(abb) == 0);

    abb_destruir(abb);
}

static void prueba_abb_clave_vacia() {
	printf("\n--PRUEBAS DE ABB CON CLAVE VACIA--\n");
    abb_t* abb = abb_crear(strcmp, NULL);

    char *clave = "", *valor = "";

    print_test("Prueba abb insertar clave vacia", abb_guardar(abb, clave, valor));
    print_test("Prueba abb la cantidad de elementos es 1", abb_cantidad(abb) == 1);
    print_test("Prueba abb obtener clave vacia es valor", abb_obtener(abb, clave) == valor);
    print_test("Prueba abb pertenece clave vacia, es true", abb_pertenece(abb, clave));
    print_test("Prueba abb borrar clave vacia, es valor", abb_borrar(abb, clave) == valor);
    print_test("Prueba abb la cantidad de elementos es 0", abb_cantidad(abb) == 0);

    abb_destruir(abb);
}

static void prueba_abb_valor_null() {
	printf("\n--PRUEBAS DE ABB CON VALOR NULL--\n");
    abb_t* abb = abb_crear(strcmp, NULL);

    char *clave = "", *valor = NULL;

    /* Inserta 1 valor y luego lo borra */
    print_test("Prueba abb insertar clave vacia valor NULL", abb_guardar(abb, clave, valor));
    print_test("Prueba abb la cantidad de elementos es 1", abb_cantidad(abb) == 1);
    print_test("Prueba abb obtener clave vacia es valor NULL", abb_obtener(abb, clave) == valor);
    print_test("Prueba abb pertenece clave vacia, es true", abb_pertenece(abb, clave));
    print_test("Prueba abb borrar clave vacia, es valor NULL", abb_borrar(abb, clave) == valor);
    print_test("Prueba abb la cantidad de elementos es 0", abb_cantidad(abb) == 0);

    abb_destruir(abb);
}

static void prueba_abb_volumen(size_t largo, bool debug) {
	printf("\n--PRUEBAS DE VOLUMEN EN EL ABB--\n");
    abb_t* abb = abb_crear(strcmp, NULL);

    const size_t largo_clave = 10;
    char* claves[largo];
    unsigned* valores[largo];

    
    for (unsigned i = 0; i < largo; i++) {
        claves[i] = malloc(largo_clave);
        valores[i] = malloc(sizeof(unsigned));
        sprintf(claves[i], "%08d", i);
        *valores[i] = i;
    }
    
    //Mezclo claves con algoritmo Fisher Yates
    FisherYates(claves, largo);

    // Inserta 'largo' parejas en el abb
    bool ok = true;
    for (unsigned i = 0; i < largo; i++) {
        ok = abb_guardar(abb, claves[i], valores[i]);
            if (!ok) break;
    }

    if (debug) print_test("Prueba abb almacenar muchos elementos", ok);
    if (debug) print_test("Prueba abb la cantidad de elementos es correcta", abb_cantidad(abb) == largo);

    // Verifica que devuelva los valores correctos 
    for (size_t i = 0; i < largo; i++) {
        ok = abb_pertenece(abb, claves[i]);
        if (!ok) break;
        ok = abb_obtener(abb, claves[i]) == valores[i];
        if (!ok) break;
    }

    if (debug) print_test("Prueba abb pertenece y obtener muchos elementos", ok);
    if (debug) print_test("Prueba abb la cantidad de elementos es correcta", abb_cantidad(abb) == largo);

    // Verifica que borre y devuelva los valores correctos 
    for (size_t i = 0; i < largo; i++) {
        ok = abb_borrar(abb, claves[i]) == valores[i];
        if (!ok) break;
    }

    if (debug) print_test("Prueba abb borrar muchos elementos", ok);
    if (debug) print_test("Prueba abb la cantidad de elementos es 0", abb_cantidad(abb) == 0);

    //Destruye el abb y crea uno nuevo que sí libera 
    abb_destruir(abb);
    abb = abb_crear(strcmp, free);

    // Inserta 'largo' parejas en el abb 
    ok = true;
    for (size_t i = 0; i < largo; i++) {
        ok = abb_guardar(abb, claves[i], valores[i]);
        if (!ok) break;
    }

    for (size_t i = 0; i < largo; i++){
        free(claves[i]);
    }
    
    // Destruye el abb - debería liberar los enteros 
    abb_destruir(abb);
}


/* ******************************************************************
 *                    PRUEBAS DEL ITERADOR INTERNO
 * *****************************************************************/

static void prueba_iter_interno_abb_vacio() {
	printf("\n--PRUEBAS DE ITERADOR INTERNO - ITERAR ABB VACIO--\n");
	abb_t* abb = abb_crear(strcmp, NULL);
	int suma = 0;
	abb_in_order(abb, sumar_elementos, &suma);
	print_test("El abb vacio iterado esta vacio", abb_cantidad(abb) == 0);
	print_test("El resultado de la suma es cero", suma == 0);
	abb_destruir(abb);
}

static void prueba_iter_interno_suma_2elem() {
	printf("\n--PRUEBAS DE ITERADOR INTERNO - SUMA DE 2 ELEMENTOS--\n");
	abb_t* abb = abb_crear(strcmp, NULL);
	int uno = 1;
	int dos = 2;
	int suma = 0;
	abb_guardar(abb, "uno", &uno);
	abb_guardar(abb, "dos", &dos);
	abb_in_order(abb, sumar_elementos, &suma);
	print_test("La suma de los elementos es 3", suma == 3);
	abb_destruir(abb);
}

static void prueba_iter_interno_suma_10elem() {
	printf("\n--PRUEBAS DE ITERADOR INTERNO - SUMA DE 10 ELEMENTOS--\n");
	abb_t* abb = abb_crear(strcmp, NULL);
	int suma = 0;
	int resultado_esperado = 45;
	size_t cant_datos = 10;
	int datos[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	char* claves[] = {"cero", "uno", "dos", "tres", "cuatro", "cinco", "seis", "siete", "ocho", "nueve"};
	//Mezclo claves con algoritmo Fisher Yates
    FisherYates(claves, cant_datos);
	for (int i = 0; i < 10; i++) {
		abb_guardar(abb, claves[i], (void*)&datos[i]);
	}
	abb_in_order(abb, sumar_elementos, &suma);
	print_test("El resultado de la suma de los elementos con el iterador interno es correcto", suma == resultado_esperado);
	abb_destruir(abb);
}

static void prueba_iter_interno_suma_primeros_3() {
	printf("\n--PRUEBA DE ITERADOR INTERNO - SUMA DE PRIMEROS 3 ELEMENTOS--\n");
	abb_t* abb = abb_crear(strcmp, NULL);
	int* suma_y_cant_iter = malloc(2 * sizeof(int));
	suma_y_cant_iter[SUMA_ELEM] = 0;
	suma_y_cant_iter[CANT_ITERACIONES] = 0;
	int datos[] = {1, 2, 3, 4, 5};
	char* claves[] = {"1", "2", "3", "4", "5"};
	for (int i = 0; i < 5; i++) {
		abb_guardar(abb, claves[i], (void*)&datos[i]);
	}
	abb_in_order(abb, suma_primeros_3, suma_y_cant_iter);
	print_test("La suma de los primeros 3 elementos es correcta", suma_y_cant_iter[SUMA_ELEM] == 6);
	abb_destruir(abb);
	free(suma_y_cant_iter);
}

static void prueba_iter_interno_iterar_hasta_palabra() {
	printf("\n--PRUEBA DE ITERADOR INTERNO - ITERAR HASTA ARMAR PALABRA--\n");
	abb_t* abb = abb_crear(strcmp, free);
	char* claves[] = {"1", "2", "3", "4", "5", "6", "7"};
	char texto[] = "casaxyz";
	char palabra[] = "casa";
	char* palabra_armada = malloc(sizeof(texto));
	*(palabra_armada) = '\0';
	char** palabras = malloc(2 * sizeof(palabra_armada));
	palabras[PALABRA] = palabra;
	palabras[PALABRA_ARMADA] = palabra_armada;
	for (size_t i = 0; i < strlen(texto); i++) {
		abb_guardar(abb, claves[i], (void*)strndup(&texto[i], 1));
	}
	abb_in_order(abb, encontrar_palabra, palabras);
	print_test("La palabra armada en la iteración es igual a la palabra a comparar", strcmp(palabras[PALABRA_ARMADA], palabras[PALABRA]) == 0);
	abb_destruir(abb);
	free(palabra_armada);
	free(palabras);
}

static void prueba_iter_interno_iterar_hasta_clave() {
	printf("\n--PRUEBA DE ITERADOR INTERNO - ITERAR HASTA ENCONTRAR CLAVE--\n");
	abb_t* abb = abb_crear(strcmp, NULL);
	char* claves[] = {"J", "E", "B", "A", "C", "G", "F", "H", "O", "L", "K", "M", "Q", "P", "R"};
	int datos[] = {10, 5, 2, 1, 3, 7, 6, 8, 15, 12, 11, 13, 17, 16, 18};
	suma_y_clave_t suma_y_clave;
	suma_y_clave.suma = 0;
	suma_y_clave.clave = claves[10];
	int resultado_esperado = 53;
	for (int i = 0; i < 15; i++) {
		abb_guardar(abb, claves[i], (void*)&datos[i]);
	}
	abb_in_order(abb, sumar_hasta_clave, &suma_y_clave);
	print_test("La suma de los valores hasta la clave especificada es correcta", suma_y_clave.suma == resultado_esperado);
	abb_destruir(abb);
}


/* ******************************************************************
 *                    PRUEBAS DEL ITERADOR EXTERNO
 * *****************************************************************/

static void prueba_iterar_abb_vacio() {
	printf("\n--PRUEBAS DE ITERADOR EXTERNO - ITERAR ABB VACIO--\n");
    abb_t* abb = abb_crear(strcmp, NULL);
    abb_iter_t* iter = abb_iter_in_crear(abb);
    print_test("Prueba abb iter crear iterador abb vacio", iter);
    print_test("Prueba abb iter esta al final", abb_iter_in_al_final(iter));
    print_test("Prueba abb iter avanzar es false", !abb_iter_in_avanzar(iter));
    print_test("Prueba abb iter ver actual es NULL", !abb_iter_in_ver_actual(iter));

    abb_iter_in_destruir(iter);
    abb_destruir(abb);
}

static void prueba_abb_iterar() {
	printf("\n--PRUEBAS DE ITERADOR EXTERNO - ITERAR ABB CON 3 ELEMENTOS--\n");
    abb_t* abb = abb_crear(strcmp, NULL);

    char *claves[] = {"perro", "gato", "vaca"};
    char *valores[] = {"guau", "miau", "mu"};

    /* Inserta 3 valores */
    print_test("Prueba abb insertar clave1", abb_guardar(abb, claves[0], valores[0]));
    print_test("Prueba abb insertar clave2", abb_guardar(abb, claves[1], valores[1]));
    print_test("Prueba abb insertar clave3", abb_guardar(abb, claves[2], valores[2]));

    // Prueba de iteración sobre las claves almacenadas.
    abb_iter_t* iter = abb_iter_in_crear(abb);
    const char *clave;
    ssize_t indice;

    print_test("Prueba abb iterador esta al final, es false", !abb_iter_in_al_final(iter));

    /* Primer valor */
    clave = abb_iter_in_ver_actual(iter);
    indice = buscar(clave, claves, sizeof(claves) / sizeof(char *));
    print_test("Prueba abb iterador ver actual, es una clave valida", indice != -1);
    print_test("Prueba abb iterador ver actual, no es el mismo puntero", clave != claves[indice]);
    print_test("Prueba abb iterador avanzar es true", abb_iter_in_avanzar(iter));
    print_test("Prueba abb iterador esta al final, es false", !abb_iter_in_al_final(iter));

    /* Segundo valor */
    clave = abb_iter_in_ver_actual(iter);
    indice = buscar(clave, claves, sizeof(claves) / sizeof(char *));
    print_test("Prueba abb iterador ver actual, es una clave valida", indice != -1);
    print_test("Prueba abb iterador ver actual, no es el mismo puntero", clave != claves[indice]);
    print_test("Prueba abb iterador avanzar es true", abb_iter_in_avanzar(iter));
    print_test("Prueba abb iterador esta al final, es false", !abb_iter_in_al_final(iter));

    /* Tercer valor */
    clave = abb_iter_in_ver_actual(iter);
    indice = buscar(clave, claves, sizeof(claves) / sizeof(char *));
    print_test("Prueba abb iterador ver actual, es una clave valida", indice != -1);
    print_test("Prueba abb iterador ver actual, no es el mismo puntero", clave != claves[indice]);
    abb_iter_in_avanzar(iter);
    print_test("Prueba abb iterador esta al final, es true", abb_iter_in_al_final(iter));

    /* Vuelve a tratar de avanzar, por las dudas */
    print_test("Prueba abb iterador ver actual, es NULL", !abb_iter_in_ver_actual(iter));
    print_test("Prueba abb iterador avanzar es false", !abb_iter_in_avanzar(iter));
    print_test("Prueba abb iterador esta al final, es true", abb_iter_in_al_final(iter));

    abb_iter_in_destruir(iter);
    abb_destruir(abb);
}

static void prueba_abb_iterar_volumen(size_t largo) {
	printf("\n--PRUEBAS DE ITERADOR EXTERNO - ITERACION EN VOLUMEN--\n");
    abb_t* abb = abb_crear(strcmp, NULL);

    const size_t largo_clave = 10;
    char* claves[largo];
    size_t valores[largo];


    for (unsigned i = 0; i < largo; i++) {
        claves[i] = malloc(largo_clave);
        sprintf(claves[i], "%08d", i);
        valores[i] = i;
    }
    
    //Mezclo claves con algoritmo Fisher Yates
    FisherYates(claves, largo);

    // Inserta 'largo' parejas en el abb
    bool ok = true;
    for (unsigned i = 0; i < largo; i++) {
        ok = abb_guardar(abb, claves[i], &valores[i]);
            if (!ok) break;
    }

    // Prueba de iteración sobre las claves almacenadas.
    abb_iter_t* iter = abb_iter_in_crear(abb);
    print_test("Prueba abb iterador esta al final, es false", !abb_iter_in_al_final(iter));

    ok = true;
    unsigned i;
    const char *clave;
    size_t *valor;

    for (i = 0; i < largo; i++) {
        if (abb_iter_in_al_final(iter)) {
            ok = false;
            break;
        }
        clave = abb_iter_in_ver_actual(iter);
        if (clave == NULL) {
            ok = false;
            break;
        }
        valor = abb_obtener(abb, clave);
        if (valor == NULL) {
            ok = false;
            break;
        }
        *valor = largo;
        abb_iter_in_avanzar(iter);
    }
    print_test("Prueba abb iteración en volumen", ok);
    print_test("Prueba abb iteración en volumen, recorrio todo el largo", i == largo);
    print_test("Prueba abb iterador esta al final, es true", abb_iter_in_al_final(iter));

    ok = true;
    for (i = 0; i < largo; i++) {
        if (valores[i] != largo) {
            ok = false;
            break;
        }
    }
    print_test("Prueba abb iteración en volumen, se cambiaron todo los elementos", ok);
    
    for (i = 0; i < largo; i++) {
        free(claves[i]);
    }
   
    abb_iter_in_destruir(iter);
    abb_destruir(abb);
}



/* ******************************************************************
 *                        FUNCIÓN PRINCIPAL
 * *****************************************************************/

void pruebas_abb_estudiante() {
    /* Ejecuta todas las pruebas unitarias. */
    prueba_crear_abb_vacio();
    prueba_abb_insertar();
    prueba_abb_reemplazar();
    prueba_abb_reemplazar_con_destruir();
    prueba_abb_borrar();
    prueba_abb_clave_vacia();
    prueba_abb_valor_null();
    prueba_abb_volumen(5000, true);
    prueba_iter_interno_abb_vacio();
    prueba_iter_interno_suma_2elem();
    prueba_iter_interno_suma_10elem();
    prueba_iter_interno_suma_primeros_3();
    prueba_iter_interno_iterar_hasta_palabra();
    prueba_iter_interno_iterar_hasta_clave();
    prueba_iterar_abb_vacio();
    prueba_abb_iterar();
    prueba_abb_iterar_volumen(5000);
}

void pruebas_volumen_estudiante(size_t largo) {
    prueba_abb_volumen(largo, false);
}


#ifndef CORRECTOR  // Para que no de conflicto con el main() del corrector.

int main(void) {
    pruebas_abb_estudiante();
    return failure_count() > 0;  // Indica si falló alguna prueba.
}

#endif

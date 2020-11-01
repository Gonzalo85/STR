//Compilación por consola: gcc p2_1.c barrera.c -o p2_1 -lpthread
//Configuración en eclipse: botón derecho sobre el proyecto -> Properties -> C/C++ Build -> Settins -> Cross GCC Linker -> Libraries -> Añadir pthread
#include <pthread.h>
#include "barrera.h"
#include "stdlib.h"

typedef struct {
	barrera_t *b;
	int idhilo; //Nos servirá para que el usuario pueda identificar los hilos
} hilo_t;

void HiloEnBarrera (hilo_t *h) {
	//Mostrar un mensaje indicando que el hilo con identificador idhilo se bloquea en la barrera
	printf("El hilo con identificador %d se BLOQUEA en la barrera\n", h->idhilo);
	//Invocar a la barrera
	sincronizar(h->b);

	//Mostrar un mensaje indicando que el hilo con identificador idhilo sale de la barrera
	printf("El hilo con identificador %d se DESBLOQUEA en la barrera\n", h->idhilo);
}

int main (int argc, char *argv[]) {
	if (argc!=2) {
		printf("Error en la ejecución. El formato correcto es %s num_hilos\n", argv[0]);
		return 0;
	}
	int num_hilos=atoi(argv[1]);
	//Crear un array de identificadores de hilos de tipo pthread_t de tamaño num_hilos
	pthread_t idHilos[num_hilos];

	//Crear una barrera e inicializarla con num_hilos
	barrera_t B;
	crea(&B,num_hilos);
	//Crear un array de tipo hilo_t de tamaño num_hilos
	hilo_t Hilos[num_hilos];

	//Para cada hilo
	for(int i=0;i<num_hilos;i++){
		//Asignar en la posición correspondiente del array de datos un identificador (único para cada hilo)
		Hilos[i].idhilo = i;
		//Asignar en la posición correspondiente del array de datos la dirección de memoria de la barrera (mismo valor para todos los hilos
		Hilos[i].b = &B; 
		
		//Crear un hilo usando la posición correspondiente del array de identificadores, la función HiloEnBarrera y la posición correspondiente del array de datos
		pthread_create(&idHilos[i], NULL, (void*) HiloEnBarrera, (void*) &Hilos[i]);
	}
	//Esperar por todos los hilos
	for(int k=0;k<num_hilos;k++){
		pthread_join(idHilos[k],NULL);
	}

		
	//Destruir la barrera
	destruye(&B);
}

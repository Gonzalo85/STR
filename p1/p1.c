#include "buffer_acotado.h"
#include <stdio.h>

#define REPETICIONES 50
#define NUM_HILOS 5

void hilo_productor(limited_buffer_t *b) {
	int num = 0;
	//Llamar REPETICIONES veces a limited_buffer_put mostrando un mensaje con el número que se introducirá, bucle for llamando a put
	for(int i=0; i<=REPETICIONES;i++){
		num= rand() % 50;//random de 0 a 50 para meter num al buffer
		limited_buffer_put(b,num);
		printf("PRODUCTOR: Se ha puesto el num %d en el buffer \n", num);
		
	}
	

}

void hilo_consumidor(limited_buffer_t *b) {
	int num=0;
	//Llamar REPETICIONES veces a limited_buffer_get mostrando un mensaje con el número leído, llamando a get
	//no usar la misma variable para el for, usar otra
	for(int i=0; i<=REPETICIONES;i++){
		limited_buffer_get(b,&num);
		printf("CONSUMIDOR: Se ha obtenido el num %d del buffer\n", num);
	} 

}


int main (void) {
	//Definir una variable limited_buffer_t
	limited_buffer_t B;

	//Inicializar el buffer
	limited_buffer_create(&B, 10);
	//Crear dos vectores idP e idC de tipo pthread_t y de tamaño NUM_HILOS para almacenar los identificadores de los productores y de los consumidores, vectores de identificadores, con pthread_create
	pthread_t idP[NUM_HILOS];
	pthread_t idC[NUM_HILOS];
	
	//Lanzar los NUM_HILOS productores y consumidores, comenzar pthread
	for(int i = 0;i<=NUM_HILOS;i++){
		pthread_create(&idP[i], NULL, (void*) hilo_productor, (void*) &B);
		pthread_create(&idC[i], NULL, (void*) hilo_consumidor, (void*) &B);
	}
	

	//Esperar a que terminen todos los productores y los consumidores, wait por todos
	for(int k = 0;k<=NUM_HILOS;k++){
		pthread_join(idP[k],NULL);
		pthread_join(idC[k],NULL);
	}
	

	//Destruir el buffer
	limited_buffer_destroy(&B);

	return 0;
}

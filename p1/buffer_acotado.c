#include "buffer_acotado.h"

//controlar buffer circular if p=n then p=0 por ejemplo o if p = (p+1)%n
//hay que reservar memoria con malloc-> int *x = int(x) malloc(10*sizeof(int)) 

void limited_buffer_create  (limited_buffer_t *buffer, int buff_size) {
	//Reservar la memoria para el campo buf del parámetro buffer usando el parámetro buff_size
	buffer->buf = (int *) malloc(buff_size*sizeof(int));

	//Inicializar los contadores y el tamaño del buffer cont a 0
	buffer->cuenta = 0;
	buffer->primero = 0;
	buffer->ultimo = 0;
	buffer->tam_buff = buff_size;

	//Inicializar el mutex, pthread_mutex_init
	pthread_mutex_init(&buffer->mutex, NULL);

	//Inicializar las variables de condición
	pthread_cond_init(&buffer->lleno, NULL);
	pthread_cond_init(&buffer->vacio, NULL);

}

void limited_buffer_destroy (limited_buffer_t  *buffer) {
	//Destruir el mutex, liberar memoria con free
	free(buffer->buf);
	pthread_mutex_destroy(&buffer->mutex);
	//Destruir las variables de condición, pthread_cond_destroy, pthread_mutex_destroy
	pthread_cond_destroy(&buffer->lleno);
	pthread_cond_destroy(&buffer->lleno);
	
	
	

}

void limited_buffer_put (limited_buffer_t  *buffer, int  elem) {
	//Acceder a la región crítica a través del mutex, pilla mutex
	pthread_mutex_lock(&buffer->mutex); //cogemos el mutex
	
	//Mientras el buffer esté lleno, bloquearse en la variable de condición lleno, cuenta no sea igual al num de elem, si esta lleno se bloquea en vacio
	while(buffer->cuenta == buffer->tam_buff){
		pthread_cond_wait(&buffer->lleno, &buffer->mutex);
	}

	//Añadir elem en el buffer y actualizar cuenta y ultimo
	buffer->buf[buffer->ultimo] = elem;
	
	//comprobacion buffer circular
	if(buffer->ultimo >= buffer->tam_buff-1){
		buffer->ultimo = 0;
	}
	else{
		buffer->ultimo++;
	}
	
	//cuenta aumenta siempre
	buffer->cuenta++;
	
	//Liberar el mutex
	pthread_mutex_unlock(&buffer->mutex);

	//Hacer un signal en la variable de condición vacio, signal sobre vacio
	pthread_cond_signal(&buffer->vacio);

}

void limited_buffer_get (limited_buffer_t  *buffer, int *elem) {
	//Acceder a la región crítica a través del mutex
	pthread_mutex_lock(&buffer->mutex);
	//Mientras el buffer esté vacío, bloquearse en la variable de condición vacio, se bloquea en vacio
	while(buffer->cuenta == 0){
		pthread_cond_wait(&buffer->vacio, &buffer->mutex);
	}
	//Guardar en *elem el primer elemento del buffer y actualizar cuenta y primero, lee elem primero, decrementamos primero y cuenta, poner *item = buf[loquesea] 
	*elem = buffer->buf[buffer->primero];
	//comprobacion buffer circular
	
	if(buffer->primero >= buffer->tam_buff-1){
		buffer->primero = 0;
	}
	else{
		buffer->primero++;
	}
	
	buffer->cuenta--;
	//Liberar el mutex
	pthread_mutex_unlock(&buffer->mutex);
	//Hacer un signal en la variable de condición lleno, hacemos signal a lleno
	pthread_cond_signal(&buffer->lleno);

}

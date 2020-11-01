//incluir la librería para usar threads
#include <pthread.h>
#include <stdlib.h>

typedef struct {
	int *buf;
	int cuenta, primero, ultimo, tam_buff;
	//Definir el mutex
	pthread_mutex_t mutex;
	//Definir las variables de condición lleno y vacio
	pthread_cond_t lleno;
	pthread_cond_t vacio;
	

} limited_buffer_t;

void limited_buffer_create  (limited_buffer_t *buffer, int buff_size);
void limited_buffer_destroy (limited_buffer_t  *buffer);
void limited_buffer_put (limited_buffer_t  *buffer, int  elem);
void limited_buffer_get (limited_buffer_t  *buffer, int *elem);

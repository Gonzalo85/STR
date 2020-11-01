#include "barrera.h"


void crea (barrera_t *b, int N) {
	//Inicializar permitidos al valor de N
	b->permitidos = N;
	//Inicializar el mutex
	pthread_mutex_init(&b->mutex,NULL);
	//Inicializar la variable de condición
	pthread_cond_init(&b->cond,NULL);
}

void destruye (barrera_t *b) {
	//Destruir el mutex
	pthread_mutex_destroy(&b->mutex);
	//Destruir la variable de condición
	pthread_cond_destroy(&b->cond);
}

void sincronizar (barrera_t *b) {//bloquear 4 en barrera
	//Entrar en el mutex
	pthread_mutex_lock(&b->mutex);//cogemos el mutex

	//Decrementar permitidos
	b->permitidos--;
	//Si permitidos es 0(if) liberar a todos los hilos(pthread_cond_broadcast) de la variable de condición, num de hilos es igual a 0 si permitidos == 0
	if(b->permitidos == 0){
		pthread_cond_broadcast(&b->cond);
	}
	else{//Si no, bloquearse en la variable de condición
		pthread_cond_wait(&b->cond,&b->mutex);
	}

	//Incrementar permitidos para que cuando salgan todos los hilos pueda volverse a usar la barrera
	b->permitidos++;
	//Liberar el mutex
	pthread_mutex_unlock(&b->mutex);
}

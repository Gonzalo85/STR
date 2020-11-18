/*
 Compilación por consola: gcc p7.c funciones_time.c -o p7 -lpthread -lrt
 Configuración en eclipse: botón derecho sobre el proyecto -> Properties -> C/C++ Build -> Settins -> Cross GCC Linker -> Libraries -> Añadir pthread y rt
 Ejecución: para poder modificar los atributos del hilo, debemos ejecutar como superusuario: sudo ./p7 valor_C_1MS modo C T
 Autor: Gonzalo Bueno Rodríguez
*/
#include <stdio.h>
#include <stdlib.h>
#include "funciones_time.h"
//Incluir librería de tiempos
#include <time.h>
//Incluir librería de Posix
#include <pthread.h>


// La función execTime mide el tiempo de ejecutar f(param) y lo devuelve en time
typedef void (*func)(void *);
int execTime(func f, void *parm, struct timespec *time){
	struct timespec t_ini, t_fin;
	clock_gettime (CLOCK_REALTIME, &t_ini);
	f(parm);
	clock_gettime(CLOCK_REALTIME, &t_fin);
	RestaTimeSpec(time, t_fin, t_ini);
	return 0;
}

//La función spin_m mantiene la ejecución durante p milisegundos
int C_1MS; /* Machine dependent */
#define DIM 100
void spin_m(void *p)
{
     int i, n = *((int *)p), limit = C_1MS * n;
     static int  a[DIM];
     for(i = 0; i < limit; i++)
           a[i%DIM] = i;
     return;
}

/*Definir una estructura marco_temp_t (usando typedef para que sea un tipo) que contenga dos campos enteros:
	T (periodo de repetición de la tarea)
	C (tiempo de ejecución de la tarea)
	La información de ambos campos estará representada en milisegundos (1000ms = 1s)
*/
typedef struct marco_temp_t{
	int T;
	int C;
}marco_temp_t;

//Definir la función TareaPeriodica que reciba el parámetro marco de tipo puntero a marco_temp_t
void TareaPeriodica(marco_temp_t* marco){

	//Definir una variable para guardar la política de la tarea
	int policy;

	//Definir tres variables (hora_actual, sig_inst y periodo) de tipo timespec
	struct timespec hora_actual, sig_inst, periodo;
	
	//Definir una variable prioridad de tipo sched_param
	struct sched_param prioridad;
	
	//Leer la hora actual
	clock_gettime(CLOCK_REALTIME, &hora_actual);
	
	//Usando la función ms2timespec de funciones_time, transformar el campo T de marco a timespec y almacenarlo en periodo
	ms2timespec(&periodo, marco->T);

	//Calcular sig_inst como hora_actual más periodo
	SumaTimeSpec(&sig_inst, hora_actual, periodo);

	//Obtener la prioridad del hilo actual
	pthread_getschedparam(pthread_self(), &policy, &prioridad);

	//Definir un bucle infinito
	while(1){
	
		//Imprimir el mensaje "Ejecutando tarea periódica con prioridad: xxxx"
		printf("Ejecutando tarea periódica con prioridad: %d\n", prioridad);

		//Ejecutar la función spin_m usando el campo C de marco como parámetro
		spin_m(&marco->C);

		//Imprimir el mensaje "Fin ejecución\n"
		printf("Fin ejecución\n");

		//Dormir el hilo hasta sig_inst
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sig_inst, NULL);
		
		//incrementar sig_inst con periodo
		AcumTimeSpec(&sig_inst, periodo);

	}//Fin bucle

}//Fin TareaPeriodica



int main(int argc, char *argv[]) {
	int modo;
	marco_temp_t marco;
	if (argc!=5) {
		printf("El formato de ejecución correcto es ./p7 valor_C_1MS modo C T\n");
		return 0;
	}
	C_1MS=atoi(argv[1]);
	modo=atoi(argv[2]);
	marco.C=atoi(argv[3]);
	marco.T=atoi(argv[4]);
	if (modo==0) {
		struct timespec time;
		execTime(spin_m, &marco.C, &time);
		ImprimirTimeSpec(time);
	}
	else if (modo==1){
		//Definir una variable prioridad de tipo sched_param
		struct sched_param prioridad;

		//Definir una variable de identificación de hilo
		pthread_t h1;
		
		//Definir una variable attr para los atributos de creación del hilo (tipo pthread_attr_t)
		pthread_attr_t attr;

		//Inicializar attr
		pthread_attr_init(&attr);

		//Indicar en attr que el hilo no heredará los atributos del padre
		pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

		//Establecer la política FIFO en attr
		pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
		
		//Asignar al campo sched_priority de prioridad la prioridad máxima FIFO
		prioridad.sched_priority = sched_get_priority_max(SCHED_FIFO);

		//Establecer la prioridad en attr
		pthread_attr_setschedparam(&attr, &prioridad);
		//Crear un hilo TareaPeriodica con el parámetro marco
		pthread_create(&h1, &attr, (void*)TareaPeriodica,&marco);

		//Esperar por la finalización del hilo
		pthread_join(h1,NULL);

	}
	else
		printf("El modo debe ser 0 o 1\n");
	return 0;
}

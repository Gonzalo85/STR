/*
 Compilación por consola: gcc p8.c funciones_time.c -o p8 -lpthread -lrt
 Configuración en eclipse: botón derecho sobre el proyecto -> Properties -> C/C++ Build -> Settins -> Cross GCC Linker -> Libraries -> Añadir pthread y rt
 Ejecución: para poder modificar los atributos del hilo, debemos ejecutar como superusuario: sudo ./p8 valor_C_1MS modo fichero_tareas
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

/*Definir una estructura marco_temp_t (usando typedef para que sea un tipo) que contenga los siguientes campos:
	T - entero (periodo de repetición de la tarea)
	C - entero (tiempo de ejecución de la tarea)
	P - entero (prioridad de la tarea)
	id - entero (identificador de la tarea)
	TCritico - timespec (instante crítico de comienzo de la tarea)

	La información de los campos T y C estará representada en milisegundos (1000ms = 1s)
*/
typedef struct marco_temp_t{
	int T;
	int C;
	int P;
	int id;
	struct timespec TCritico;
}marco_temp_t;

//Definir la función TareaPeriodica que reciba el parámetro marco de tipo puntero a marco_temp_t
void TareaPeriodica(marco_temp_t* marco){

	//Definir dos variables sig_inst y periodo de tipo timespec
	struct timespec sig_inst, periodo;

	//Definir una variable prioridad de tipo sched_param
	struct sched_param prioridad;
	
	//asignar el valor del campo P del parámetro marco al campo sched_priority de la variable prioridad
	prioridad.sched_priority = marco->P;
	
	//Usando la función ms2timespec de funciones_time, transformar el campo T del parámetro marco a timespec y almacenarlo en la variable periodo
	ms2timespec(&periodo, marco->T);

	//Calcular sig_inst como TCritico (campo del parámetro marco) más periodo
	SumaTimeSpec(&sig_inst, marco->TCritico, periodo);

	//Establecer la prioridad (variable prioridad) y la poĺítica (FIFO) del hilo actual
	pthread_setschedparam(pthread_self(), SCHED_FIFO, &prioridad);

	//Dormir el hilo hasta el instante crítico (campo TCritico del parámetro marco)
	clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &marco->TCritico,NULL);

	//Definir un bucle infinito
	while(1){

		//Imprimir el mensaje "Hilo id con prioridad P comienza su acción periódica"
			printf("Hilo %d con prioridad %d comienza su acción periódica\n",marco->id, marco->P);

		//Ejecutar la función spin_m usando como parámetro el campo C del parámetro marco (debe ser puntero)
		spin_m(&marco->C);

		//Imprimir el mensaje "Hilo id termina su acción periódica\n"
		printf("Hilo %d con prioridad %d termina su acción periódica\n",marco->id, marco->P);
		
		//Dormir el hilo hasta sig_inst
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sig_inst, NULL);
		
		//incrementar sig_inst con periodo
		AcumTimeSpec(&sig_inst, periodo);

		}//Fin bucle

}//Fin TareaPeriodica



//Definir la función tareaMostrarTiempos que reciba el parámetro TCritico de tipo puntero a timespec
void tareaMostrarTiempos(struct timespec* TCritico){

	//Definir dos variables sig_inst y periodo de tipo timespec
	struct timespec sig_inst, periodo;

	//Usando la función ms2timespec de funciones_time, establecer el valor de la variable periodo a 10ms
	ms2timespec(&periodo, 10);
	
	//Calcular sig_inst como TCritico más periodo. Como TCritico es un puntero y SumaTimeSpec recibe un valor no puntero, debemos usar *TCritico en la llamada
	SumaTimeSpec(&sig_inst, *TCritico, periodo);

	//Dormir el hilo hasta TCritico
	clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, TCritico, NULL);

	//Para i desde 0 hasta 200
	for(int i = 0 ; i <= 200 ; i++){

		//Imprimir "************************* x ms *************************\n", siendo x=i*10
		printf("************************* %d ms *************************\n",i*10);
		
		//Dormir el hilo hasta sig_inst
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sig_inst, NULL);
		//incrementar sig_inst con periodo
		AcumTimeSpec(&sig_inst, periodo);

	}//Fin Para

}//Fin tareaMostrarTiempos


int main(int argc, char *argv[]) {
	marco_temp_t *marcos;
	int num_tareas, i, modo;
	FILE *f;
	if (argc!=4) {
		printf("El formato de ejecución correcto es %s valor_C_1MS modo fichero_tareas\n", argv[0]);
		return 0;
	}
	C_1MS=atoi(argv[1]);
	modo=atoi(argv[2]);
	if (modo==0) {
		struct timespec time;
		int t=1000;
		execTime(spin_m, &t, &time);
		ImprimirTimeSpec(time);
	}
	else if (modo==1) {
		f=fopen(argv[3], "r");
		if(f==NULL) {
			printf("Error al abrir el fichero %s\n", argv[3]);
			return 0;
		}
		fscanf(f, "%d\n", &num_tareas);
		marcos=(marco_temp_t*)malloc(sizeof(marco_temp_t)*num_tareas);
		for (i=0; i<num_tareas; i++) {
			fscanf(f, "%d %d %d\n", &marcos[i].C, &marcos[i].T, &marcos[i].P);
			marcos[i].id=i+1;
			printf("t%d: %d %d %d\n", marcos[i].id, marcos[i].C, marcos[i].T, marcos[i].P);
		}
		//Definir una variable TCritico de tipo timespec
		struct timespec TCritico;

		//Leer la hora actual y almacenarla en TCritico
		clock_gettime(CLOCK_REALTIME, &TCritico);
		//Sumar 2 segundos a TCritico (se puede sumar 2 directamente al campo tv_sec de TCritico)
		TCritico.tv_sec = TCritico.tv_sec + 2;

		//Para i desde 0 hasta num_tareas-1
		for( int i = 0; i<num_tareas-1; i++){
			//Asignar la variable TCritico al campo TCritico del marco i-ésimo
			marcos[i].TCritico = TCritico;
		}
		
		//Definir una variable prioridad de tipo sched_param
		struct sched_param prioridad;
		
		//Definir dos variables de identificación de hilo, una para la tarea tareaMostrarTiempos y otra que se usará para todas las tareas tareaPeriodica (sólo vamos a esperar por el hilo tareaMostrarTiempos)
		pthread_t idHiloMostrar, idHiloPeriodica;
		
		//Definir una variable attr de tipo pthread_attr_t para los atributos de creación de hilo
		pthread_attr_t attr;

		//Inicializar attr
		pthread_attr_init(&attr);

		//Indicar en attr que el hilo no heredará los atributos del padre
		pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

		//Establecer la política FIFO en attr
		pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

		//Asignar al campo sched_priority de la variable prioridad la prioridad máxima FIFO
		prioridad.sched_priority = sched_get_priority_max(SCHED_FIFO);
		
		//Establecer la prioridad en attr
		pthread_attr_setschedparam(&attr, &prioridad);

		//Crear un hilo tareaMostrarTiempos con el parámetro TCritico y los atributos que acabamos de crear (attr)
		pthread_create(&idHiloMostrar, &attr, (void*)tareaMostrarTiempos, &TCritico);

		//Para i desde 0 hasta num_tareas-1
		for(i = 0; i<num_tareas-1; i++){

			//Crear un hilo tareaPeriodica con el marco temporal i-ésimo y con NULL como atributos de creación de hilo (los valores se establecen dentro de la propia tarea)
			pthread_create(&idHiloPeriodica, NULL, (void*)TareaPeriodica, &marcos[i]);
		}	

		//Esperar únicamente por la finalización del hilo tareaMostrarTiempos
		pthread_join(idHiloMostrar,NULL);

		free(marcos);
	}
	else
		printf("El modo debe tener los valores 0 o 1\n");
	return 0;
}

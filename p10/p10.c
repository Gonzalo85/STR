/*
 Compilación por consola: gcc p10.c funciones_time.c -o p10 -lpthread -lrt
 Configuración en eclipse: botón derecho sobre el proyecto -> Properties -> C/C++ Build -> Settins -> Cross GCC Linker -> Libraries -> Añadir pthread y rt
 Ejecución: para poder modificar los atributos del hilo, debemos ejecutar como superusuario: sudo ./p10 fichero_tareas
 Autor: Gonzalo Bueno Rodriguez
*/
#include <stdio.h>
#include <stdlib.h>
#include "funciones_time.h"
//Incluir librería de tiempos
#include <time.h>
//Incluir librería de Posix
#include <pthread.h>


//La función spin_m mantiene la ejecución durante p milisegundos
void spin_m(int n)
{
     int i;
	struct timespec ini, fin, dif;
	int finb=0;
	double y=n/1000.0+0.0005;
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ini);
	while (!finb) {
		clock_gettime(CLOCK_THREAD_CPUTIME_ID, &fin);
		RestaTimeSpec(&dif, fin, ini);
		if (timespec2segundos(dif)>=y)
			finb=1;
	}
}
/*Definir una estructura marco_temp_t (usando typedef para que sea un tipo) que contenga los siguientes campos:
	T - entero (periodo de repetición de la tarea)
	C - entero (tiempo de ejecución de la tarea)
	P - entero (prioridad de la tarea)
	id - entero (identificador de la tarea)
	Ta - entero (tiempo de comienzo de la tarea. Empezará a contar a partir del instante crítico)
	num_acc - entero (número de acciones que realiza la tarea)
	acciones - puntero a entero (contendrá la lista de tiempos de ejecución, por orden de ejecución, de las acciones de la tarea)
	recursos - puntero a entero (contendrá la lista de recursos, por orden de ejecución, que usa la tarea.
							   los recursos compartidos tendrán valores mayores o iguales que 0, mientras que
							   para indicar el uso de tiempo de ejecución no compartido se usará el valor -1. El par
							   (recursos[x], acciones[x]) nos indica el recurso y el tiempo de ejecución de la acción x-ésima)
	TCritico - timespec (instante crítico de comienzo de la tarea. Los tiempos de comienzo de cada tarea se cuentan a partir de aquí)
	mutex - puntero a pthread_mutex_t (lista de los mutex utilizados en la aplicación. Es común para todas las tareas.
								   Cada recurso compartido tendrá asignado un mutex)

	La información de los campos T, C y Ta estará representada en milisegundos (1000ms = 1s)
*/
typedef struct marco_temp_t{
	int T;
	int C;
	int P;
	int id;
	int Ta;
	int num_acc;
	int* acciones;
	int* recursos;
	struct timespec TCritico;
	pthread_mutex_t* mutex;
}marco_temp_t;

//Definir la función EjecutarRecurso con los siguientes parámetros:
  //mutex - puntero a pthread_mutex_t (lista de los mutex utilizados en la aplicación)
  //recurso - entero (indica el recurso que se va a ejecutar: -1 no compartido, >=0 compartido)
  //tiempo - entero (tiempo de ejecución del recurso)
  //id - entero (identificador de la tarea que realiza la ejecución)
void EjecutarRecurso(pthread_mutex_t* mutex, int recurso, int tiempo, int id){

	//Si "recurso" es un recurso compartido
	if(recurso != -1){
		//bloquear el mutex indicado por "recurso"
		pthread_mutex_lock(&mutex[recurso]);
	}
	//Imprimir el mensaje "\t\t\tHilo id comienza la ejecución del recurso x que requiere y ms.", donde "id" es el identificador de la tarea que 
	//ejecuta el recurso, "x" es el identificador del recurso e "y" es el tiempo de ejecución del recurso
	printf("\t\t\tHilo %d comienza la ejecución del recurso %d que requiere %d ms.\n",id, recurso, tiempo);
	
	//Ejecutar spin_m con el tiempo de ejecución del recurso
	spin_m(tiempo);
	
	//Mostrar el mensaje "\t\t\tHilo id termina con el recurso x.", donde "id" es el identificador de la tarea que 
	//ejecuta el recurso y "x" es el identificador del recurso
	printf("\t\t\tHilo %d termina con el recurso %d\n",id, recurso);
	//Si "recurso" es un recurso compartido
	if(recurso != -1){
		//desbloquear el mutex indicado por "recurso"
		pthread_mutex_unlock(&mutex[recurso]);
	}
}//Fin EjecutarRecurso


//Definir la función tareaPeriodica que recibe el parámetro marco de tipo puntero a marco_temp_t
void TareaPeriodica(marco_temp_t* marco){

	//Definir las variables "sig_inst", "periodo" e "inicio" de tipo timespec
	struct timespec sig_inst, periodo, inicio;
	
	//Definir una variable "prioridad" de tipo sched_param
	struct sched_param prioridad;
	
	//asignar el valor del campo "P" del parámetro "marco" al campo "sched_priority" de la variable "prioridad"
	prioridad.sched_priority = marco->P;
	
	//Usando la función ms2timespec de funciones_time, transformar el campo "T" del parámetro "marco" a timespec y almacenarlo en la variable "periodo"
	ms2timespec(&periodo, marco->T);
	
	//Usando la función ms2timespec de funciones_time, transformar el campo "Ta" del parámetro "marco" a timespec y almacenarlo en la variable "inicio"
	ms2timespec(&inicio, marco->Ta);
	
	//Calcular "sig_inst" como "TCritico" (campo del parámetro marco) más "inicio". Este será el instante de comienzo de la tarea: TCrítico+Ta
	SumaTimeSpec(&sig_inst, marco->TCritico, inicio);
	
	//Establecer la prioridad (variable "prioridad") y la poĺítica (FIFO) del hilo actual
	pthread_setschedparam(pthread_self(), SCHED_FIFO, &prioridad);
	
	//Dormir el hilo hasta el instante de comienzo (variable "sig_inst")
	clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sig_inst,NULL);
	
	//Calcular el siguiente instante de ejecución como "sig_inst" más "periodo"
	AcumTimeSpec(&sig_inst, periodo);
	
	//Definir un bucle infinito
	while(1){
		//Imprimir el mensaje "Hilo id con prioridad P comienza su acción periódica"
		printf("Hilo %d con prioridad %d comienza su acción periódica\n",marco->id, marco->P);

		//Para cada acción del marco temporal
		for(int i=0; i<marco->num_acc;i++){
			//Invocar a la función EjecutarRecurso con los campos correspondientes del marco temporal: la lista de mutex, el recurso i-ésimo, la acción i-ésima y el identificador de la tarea
			EjecutarRecurso(marco->mutex,marco->recursos[i],marco->acciones[i],marco->id);
		}
			
		//Imprimir el mensaje "Hilo id termina su acción periódica"
		printf("Hilo %d termina su acción periódica\n",marco->id);
		
		//Dormir el hilo hasta "sig_inst"
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sig_inst, NULL);
		
		//incrementar "sig_inst" con "periodo"
		AcumTimeSpec(&sig_inst, periodo);

	}//Fin bucle
	
}//Fin tareaPeriodica


//Definir la función tareaMostrarTiempos que reciba el parámetro "TCritico" de tipo puntero a timespec
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
	int num_tareas, i, j, num_recursos, *techos_prioridad;
	FILE *f;
	if (argc!=2) {
		printf("El formato de ejecución correcto es %s vfichero_tareas\n", argv[0]);
		return 0;
	}
	f=fopen(argv[1], "r");
	if(f==NULL) {
		printf("Error al abrir el fichero %s\n", argv[1]);
		return 0;
	}
	fscanf(f, "%d\n", &num_tareas);
	marcos=(marco_temp_t*)malloc(sizeof(marco_temp_t)*num_tareas);
	fscanf(f, "%d", &num_recursos);
	techos_prioridad=(int *)malloc(sizeof(int)*num_recursos);
	for (i=0; i<num_recursos; i++) {
		fscanf(f, " %d", &techos_prioridad[i]);
		printf("Techo recurso %d: %d\n", i+1, techos_prioridad[i]);
	}

	printf ("Tarea\tC\tT\tTa\tP\tacciones\n");
	for (i=0; i<num_tareas; i++) {
		fscanf(f, "%d %d %d %d %d ", &marcos[i].C, &marcos[i].T, &marcos[i].Ta, &marcos[i].P, &marcos[i].num_acc);
		marcos[i].id=i+1;
		printf("t%d:\t%d\t%d\t%d\t%d\t%d - ", marcos[i].id, marcos[i].C, marcos[i].T, marcos[i].Ta, marcos[i].P, marcos[i].num_acc);
		char c;
		marcos[i].acciones=(int *) malloc(sizeof(int)*marcos[i].num_acc);
		marcos[i].recursos=(int *) malloc(sizeof(int)*marcos[i].num_acc);
		for (j=0; j<marcos[i].num_acc; j++) {
			fscanf(f, " %c_%d", &c, &marcos[i].acciones[j]);
			if (c=='n' || c=='N')
				marcos[i].recursos[j]=-1;
			else
				marcos[i].recursos[j]=c-48;
			printf("R%d %dms ", marcos[i].recursos[j], marcos[i].acciones[j]);
		}
		fscanf(f, "\n");
		printf("\n");
	}
	
	/*
	Variables predefinidas y su significado:
		marcos: vector de marco_temp_t, contiene los marcos temporales de todas las tareas
		num_tareas: número de tareas que se van a ejecutar
		num_recursos: número de recursos compartidos existentes
		techos_prioridad: vector de enteros con los techos de prioridad de los recursos compartidos
	*/

	//Definir la variable "mutex" de tipo puntero a pthread_mutex_t (lista de mutex de la aplicación)
	pthread_mutex_t* mutex;
	
	//Reservar memoria para la variable "mutex" (sizeof(pthread_mutex_t)*num_recursos)
	mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t)*num_recursos);
	
	//Definir la variable "mutex_attr" de tipo puntero a pthread_mutexattr_t (lista de atributos de creación de los mutex
	pthread_mutexattr_t* mutex_attr;
	
	//Reservar memoria para la variable "mutex_attr" (sizeof(pthread_mutexattr_t)*num_recursos)
	mutex_attr = (pthread_mutexattr_t*) malloc(sizeof(pthread_mutexattr_t)*num_recursos);
	
	//Para i desde 0 hasta num_recursos-1
	for(int i = 0; i<num_recursos ; i++){
		
		//Inicializar el atributo de mutex i-ésimo
		pthread_mutexattr_init(&mutex_attr[i]);
		
		//Establecer el protocolo Techo de Prioridad Inmediato en el atributo de mutex i-ésimo
		pthread_mutexattr_setprotocol(&mutex_attr[i], PTHREAD_PRIO_PROTECT);
		
		//Establecer el techo de prioridad del atributo de mutex i-ésimo con el techo de prioridad i-ésimo (vector "techos_prioridad")
		pthread_mutexattr_setprioceiling(&mutex_attr[i], techos_prioridad[i]);
		
		//Inicializar el mutex i-ésimo con el atributo de mutex i-ésimo
		pthread_mutex_init(&mutex[i], &mutex_attr[i]);
	
	}//Fin para

	//Definir una variable "TCritico" de tipo timespec
	struct timespec TCritico;

	//Leer la hora actual y almacenarla en "TCritico"
	clock_gettime(CLOCK_REALTIME, &TCritico);
	
	//Sumar 2 segundos a "TCritico" (se puede sumar 2 directamente al campo tv_sec de TCritico)
	TCritico.tv_sec = TCritico.tv_sec + 2;
	
	//Para i desde 0 hasta num_tareas-1
	for( int i = 0; i<num_tareas; i++){
		//Asignar la variable TCritico al campo TCritico del marco i-ésimo
		marcos[i].TCritico = TCritico;
		
		//Asignar la variable "mutex" al campo "mutex" del marco i-ésimo
		marcos[i].mutex = mutex;
	}//Fin para

	//Definir una variable "prioridad" de tipo sched_param
	struct sched_param prioridad;
	
	//Definir dos variables de identificación de hilo, una para la tarea tareaMostrarTiempos y otra que se usará para todas las tareas tareaPeriodica (sólo vamos a esperar por el hilo tareaMostrarTiempos)
	pthread_t idHiloMostrar, idHiloPeriodica;
	
	//Definir una variable "attr" de tipo pthread_attr_t para los atributos de creación de hilo
	pthread_attr_t attr;
	
	//Inicializar "attr"	
	pthread_attr_init(&attr);	

	//Indicar en "attr" que el hilo no heredará los atributos del padre
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	
	//Establecer la política FIFO en "attr"
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	
	//Asignar al campo sched_priority de la variable "prioridad" la prioridad máxima FIFO
	prioridad.sched_priority = sched_get_priority_max(SCHED_FIFO);
	
	//Establecer la prioridad en "attr"
	pthread_attr_setschedparam(&attr, &prioridad);
	
	//Crear un hilo tareaMostrarTiempos con el parámetro "TCritico" y los atributos que acabamos de crear (variable "attr")
	pthread_create(&idHiloMostrar, &attr, (void*)tareaMostrarTiempos, &TCritico);
	
	//Para i desde 0 hasta num_tareas-1
	for(i = 0; i<num_tareas; i++){
		//Crear un hilo tareaPeriodica usando el marco temporal i-ésimo y con NULL como atributo de creación de hilo (los valores se establecen dentro de la propia tarea)
		pthread_create(&idHiloPeriodica, NULL, (void*)TareaPeriodica, &marcos[i]);

	}//Fin para

	//Esperar únicamente por la finalización del hilo tareaMostrarTiempos
	pthread_join(idHiloMostrar,NULL);
	
	//Destruir la variable attr así como todos los elementos de los vectores "mutex" y "mutex_attr"

	for (i=0; i<num_tareas; i++) {
		free(marcos[i].acciones);
		free(marcos[i].recursos);
	}
	free(marcos);
	free(mutex);
	return 0;
}

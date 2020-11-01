//Compilación por consola: gcc p5.c funciones_time.c -o p5 -lpthread -lrtf
//Configuración en eclipse: botón derecho sobre el proyecto -> Properties -> C/C++ Build -> Settins -> Cross GCC Linker -> Libraries -> Añadir pthread y rt(time.h?)
//Autor:Gonzalo Bueno Rodríguez
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
//Añadir el include de las señales
#include <signal.h>
//Añadir el include de los hilos
#include <pthread.h>
//Añadir el include de los relojes

#include "funciones_time.h"

//Consideraciones sobre las horas mostradas con printf:
//Sumamos 1900 al al año porque tm_year da los años transcurridos desde 1900
//El mes se muestra con 1 menos al actual ya que existe el mes 0, los meses van de 0 a 11 no de 1 a 12
//Si sumamos 1 a los meses nos mostraría el mes actual correctamente
//CLOCK_MONOTONIC nos da la el tiempo que lleva la máquina encendida desde la época UNIX, que es 1/01/1970(se muestra mes 0 que es 1 en realidad)
//CLOCK_REALTIME es la hora actual de la máquina, teniendo en cuenta que los meses van de 0 a 11 en el struct tm
//CLOCK_THREAD_CPUTIME_ID es el tiempo del procesador de la hebra que lo invoca

typedef struct {
	int recibidas, esperadas;
} seniales_t;

void HiloSigInt(seniales_t *seniales) {
	//Crear un conjunto de señales
	sigset_t set;
	int sig;//cualquiera recibida se guarda aquí

	//Inicializar el conjunto de señales a vacío
	sigemptyset(&set);
	
	//Añadir SIGINT al conjunto de señales
	sigaddset(&set,SIGINT);	

	//Añadir SIGRTMIN al conjunto de señales
	sigaddset(&set,SIGRTMIN);
	
	//Añadir el conjunto de señales a la máscara de señales bloqueadas
	pthread_sigmask(SIG_BLOCK, &set, NULL);
	
	//Quitar SIGRTMIN del conjunto de señales. Esto es necesario ya que, aunque bloqueamos las dos señales, solamente esperamos por SIGINT
	sigdelset(&set, SIGRTMIN);

	//Mientras las señales recibidas sean menos que las esperadas
	while(seniales->recibidas < seniales->esperadas){
		//Esperar por el conjunto de señales (solamente contiene SIGINT)
		if(sigwait(&set,&sig)==0){//recibida correctamente, solo está SIGINT en el set
			//Incrementar el número de señales recibidas
			seniales->recibidas++;
		}
	
	}//Fin mientras

}

void HiloPeriodicoConRetardos(seniales_t *seniales) {
	//Crear un conjunto de señales
	sigset_t set;

	//Inicializar el conjunto de señales a vacío
	sigemptyset(&set);

	//Añadir SIGINT al conjunto de señales
	sigaddset(&set,SIGINT);

	//Añadir SIGRTMIN al conjunto de señales
	sigaddset(&set,SIGRTMIN);

	//Añadir el conjunto de señales a la máscara de señales bloqueadas
	pthread_sigmask(SIG_BLOCK, &set, NULL);

	//Definir tres variables de tipo struct timespec para almacenar la lectura de la hora actual, el instante siguiente y el periodo de repetición
	struct timespec actual, siguiente, periodo;
	
	/*Definir una variable de tipo puntero a struct tm para almacenar la transformación de la hora en formato calendario (no se debe reservar memoria).
	Debe ser de tipo puntero porque la función localtime devuelve la dirección de memoria con la conversión.*/
	struct tm* hora;

	//Leer la hora actual usando CLOCK_MONOTONIC
	clock_gettime(CLOCK_MONOTONIC, &actual);

	//Inicializar el periodo de repetición a 5 segundos (hay que inicializar también los nanosegundos)
	segundos2timespec(&periodo,5.0);

	//Calcular el siguiente instante como la suma de la hora actual y el periodo de repetición (para hacer la suma, usar las funciones definidas en la librería funciones_time)
	SumaTimeSpec(&siguiente, actual, periodo);

	//Mientras las señales recibidas sean menos que las esperadas
	while(seniales->recibidas < seniales->esperadas){
	

		//Leemos la hora actual de CLOCK_MONOTONIC
		clock_gettime(CLOCK_MONOTONIC, &actual);

		//Transformamos la hora actual a formato struct tm (usar la variable definida fuera del bucle) mediante la función localtime (usar los segundos de la hora actual)
		hora = localtime(&actual.tv_sec);

		//Mostrar la hora en formato día/mes/año horas:minutos:segundos.milisegundos. Los milisegundos se obtienen al dividir los nanosegundos de la hora actual por 1000000
		
		printf("%d/%d/%d %d:%d:%d.%ld \n",hora->tm_mday, hora->tm_mon, hora->tm_year+1900, hora->tm_hour, hora->tm_min, 
		hora->tm_sec,actual.tv_nsec/1000000); 

		//Leemos la hora actual de CLOCK_REALTIME
		clock_gettime(CLOCK_REALTIME, &actual);

		//Transformamos la hora actual a formato struct tm (usar la variable definida fuera del bucle) mediante la función localtime (usar los segundos de la hora actual)
		hora = localtime(&actual.tv_sec);

		//Mostrar la hora en formato día/mes/año horas:minutos:segundos.milisegundos. Los milisegundos se obtienen al dividir los nanosegundos de la hora actual por 1000000
		printf("%d/%d/%d %d:%d:%d.%ld \n",hora->tm_mday, hora->tm_mon, hora->tm_year+1900, hora->tm_hour, hora->tm_min, 
		hora->tm_sec,actual.tv_nsec/1000000); 

		//Leemos la hora actual de CLOCK_THREAD_CPUTIME_ID
		clock_gettime(CLOCK_THREAD_CPUTIME_ID, &actual);

		//Transformamos la hora actual a formato struct tm (usar la variable definida fuera del bucle) mediante la función localtime (usar los segundos de la hora actual)
		hora = localtime(&actual.tv_sec);

		//Mostrar la hora en formato día/mes/año horas:minutos:segundos.milisegundos. Los milisegundos se obtienen al dividir los nanosegundos de la hora actual por 1000000
		printf("%d/%d/%d %d:%d:%d.%ld \n",hora->tm_mday, hora->tm_mon, hora->tm_year+1900, hora->tm_hour, hora->tm_min, 
		hora->tm_sec,actual.tv_nsec/1000000); 

		//Crear un bucle que realice 100000000 de vueltas (el cuerpo del bucle estará vacío). Esta acción incrementará el tiempo de procesador del hilo debido a las sumas y comparaciones del buble
		for(int i = 0; i<100000000; i++);

		//Dormir el hilo hasta el siguiente instante calculado usando CLOCK_MONOTONIC y retardo absoluto
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &siguiente, NULL);

		//Calcular el siguiente instante añadiendo el periodo al valor calculado previamente (para hacer la suma, usar las funciones definidas en la librería funciones_time)
		AcumTimeSpec(&siguiente, periodo);

	}//Fin mientras

}

void HiloPeriodicoConTemporizador(seniales_t *seniales){
	//Crear un conjunto de señales
	sigset_t set;
	int sig;

	//Inicializar el conjunto de señales a vacío
	sigemptyset(&set);

	//Añadir SIGINT al conjunto de señales
	sigaddset(&set,SIGINT);

	//Añadir SIGRTMIN al conjunto de señales
	sigaddset(&set,SIGRTMIN);

	//Añadir el conjunto de señales a la máscara de señales bloqueadas
	pthread_sigmask(SIG_BLOCK, &set, NULL);

	//Quitar SIGINT del conjunto de señales. Esto es necesario ya que, aunque bloqueamos las dos señales, solamente esperamos por SIGRTMIN
	sigdelset(&set, SIGINT);

	//Definir una variable id_timer de tipo timer_t que sirva de identificador del temporizador
	timer_t id_timer;

	//Definir una variable evento de tipo struct sigevent para indicar la acción del temporizador
	struct sigevent evento;

	//Indicar en la variable evento que el temporizador generará una señal
	evento.sigev_notify = SIGEV_SIGNAL;

	//Indicar en la variable evento que la señal generada por el temporizador es SIGRTMIN
	evento.sigev_signo = SIGRTMIN;

	//Crear el temporizador usando CLOCK_MONOTONIC, evento e id_timer
	timer_create(CLOCK_MONOTONIC, &evento, &id_timer);

	//Definir una variable datos_temporizador de tipo struct itimerspec
	struct itimerspec datos_temporizador;

	//Leer la hora actual de CLOCK_MONOTONIC y almacenarla en el campo it_value de datos_temporizador
	clock_gettime(CLOCK_MONOTONIC, &datos_temporizador.it_value);

	//Añadir dos segundos al campo it_value de datos_temporizador
	datos_temporizador.it_value.tv_sec += 2.0;

	//Establecer el campo it_interval de datos_temporizador a 2 segundos (hay que inicializar también los nanosegundos)
	datos_temporizador.it_interval.tv_sec = 2.0;
	datos_temporizador.it_interval.tv_nsec = 0.0;

	//Armar el temporizador usando id_timer, retardos absolutos y datos_temporizador (no es necesario guardar el tiempo restante en caso de salida prematura, por lo que se puede usar NULL)
	timer_settime(id_timer, TIMER_ABSTIME, &datos_temporizador, NULL);

	//Mientras las señales recibidas sean menos que las esperadas
	while(seniales->recibidas < seniales->esperadas){

		//Imprimir el número de señales recibidas actualmente
		printf("Señales recibidas: %d \n", seniales->recibidas);

		//Esperar por la señal SIGRTMIN, la guardamos en &sig
		sigwait(&set,&sig);
		

	}//Fin mientras

}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf(
				"Error en la ejecución. El formato correcto es %s num_capturas\n", argv[0]);
		return 0;
	}
	seniales_t seniales;
	seniales.esperadas= atoi(argv[1]);
	seniales.recibidas=0;

	//Crear un conjunto de señales
	sigset_t set;

	//Inicializar el conjunto de señales a vacío
	sigemptyset(&set);

	//Añadir SIGINT al conjunto de señales
	sigaddset(&set,SIGINT);

	//Añadir SIGRTMIN al conjunto de señales
	sigaddset(&set,SIGRTMIN);

	//Añadir el conjunto de señales a la máscara de señales bloqueadas
	pthread_sigmask(SIG_BLOCK, &set, NULL);

	//Crear identificadores de hilo de tipo pthread_t para todos los hilos
	pthread_t idHilo1;
	pthread_t idHilo2;
	pthread_t idHilo3;

	//Crear un hilo con la función HiloSigInt y el parámetro seniales
	pthread_create(&idHilo1, NULL, (void*) HiloSigInt, &seniales);

	//Crear un hilo con la función HiloPeriodicoConRetardos y el parámetro seniales
	pthread_create(&idHilo2, NULL, (void*) HiloPeriodicoConRetardos, &seniales);

	//Crear un hilo con la función HiloPeriodicoConTemporizador y el parámetro seniales
	pthread_create(&idHilo3, NULL, (void*) HiloPeriodicoConTemporizador, &seniales);

	//Esperar al hilo HiloSigInt
	pthread_join(idHilo1,NULL);

	//Esperar al hilo HiloPeriodicoConRetardos
	pthread_join(idHilo2,NULL);

	//Esperar al hilo HiloPeriodicoConTemporizador
	pthread_join(idHilo3,NULL);

}



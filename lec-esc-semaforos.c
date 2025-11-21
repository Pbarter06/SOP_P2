#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


#define MAX_L  3
#define MAX_E  2


int dato = 0;
sem_t mutex, m_escritores; 
int nlectores = 0; 


void *lector(void *arg) {


    int id = (int )arg;
    while (1) {

        sem_wait(&mutex);
        nlectores++; 

        if(nlectores == 1){
            sem_wait(&m_escritores); 
        }
        sem_post(&mutex);

        // Leer datos
        printf("Lector %d leyendo: %d\n", id, dato);
        usleep(rand() % 500000); //Retraso aleatorio de hasta 0.5 segundos


        sem_wait(&mutex); 
        nlectores--; 

        if(nlectores == 0){
            sem_post(&m_escritores);
        }
        
        sem_post(&mutex);
        //El lector hace otras cosas
        usleep(rand() % 1000000); // Retraso aleatorio de hasta 1 segundo (en microsegundos)
    }
}

void *escritor(void *arg) {
    int id = (int )arg;
     int aux;
    while (1) {


        sem_wait(&m_escritores);

        // Escribir datos
        aux = dato;
        usleep(rand() % 5000);
        aux++;
        usleep(rand() % 5000);
        dato = aux;
        usleep(rand() % 5000);
        printf("Escritor %d escribiendo: %d\n", id, dato);
        sem_post(&m_escritores);

        //el escritor hace otras cosas
        usleep(rand() % 2000000); // Retraso aleatorio de hasta 2 segundos (en microsegundos)

        
    }
}

int main() {

    sem_init(&mutex, 0, 1); 
    sem_init(&m_escritores, 0, 1); 

    // MAX_L lectores y MAX_E escritores
    pthread_t lectores[MAX_L], escritores[MAX_E]; 
    int i;
    pthread_attr_t atrib;

    // Inicializar la semilla del generador de números aleatorios
    srand(time(NULL)); 

    // Inicializar la estructura pthread_attr
   pthread_attr_init(&atrib); 

    // Crear lectores
    for(int i = 0; i < MAX_L; i++){
        pthread_create(&lectores[i], &atrib, lector, (void *) i);
    }

    // Crear escritores
    for(int i = 0; i < MAX_E; i++){
        pthread_create(&escritores[i], &atrib, escritor, (void *) i);
    }

    // Esperar a que los hilos terminen
    for(int i = 0; i < MAX_L; i++){
        pthread_join(lectores[i], NULL);
    }

    for(int i = 0; i < MAX_E; i++){
        pthread_join(escritores[i], NULL);
    }
    
    printf("Acaba el main\n");
    return 0;
}

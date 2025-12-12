#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


#define MAX_L  3
#define MAX_E  4

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER; 
int n_lect = 0; 
pthread_cond_t cond_lect = PTHREAD_COND_INITIALIZER , cond_escr = PTHREAD_COND_INITIALIZER ;

int dato = 0, nesc_esperando = 0, nesc_activos = 0;


void inicio_lect(){
    pthread_mutex_lock(&m);

    while (nesc_activos > 0 || nesc_esperando > 0) {
        pthread_cond_wait(&cond_lect,&m);
    }
    
    n_lect++; 

    pthread_mutex_unlock(&m); 
}


void fin_lect(){
    pthread_mutex_lock(&m);

    n_lect--; 

    if(n_lect == 0){
        pthread_cond_broadcast(&cond_escr); 
    }
    
    pthread_mutex_unlock(&m); 

}

void inicio_esc(){
    pthread_mutex_lock(&m);

    nesc_esperando++; 

    while(n_lect > 0 || nesc_activos > 0){
        pthread_cond_wait(&cond_escr, &m); 
    }
        
    nesc_esperando--; 
    nesc_activos++; 

    pthread_mutex_unlock(&m); 
}

void fin_esc(){
    pthread_mutex_lock(&m);

    if(nesc_esperando == 0) pthread_cond_broadcast(&cond_lect); 
    pthread_cond_broadcast(&cond_escr); 

    nesc_activos--; 
    pthread_mutex_unlock(&m); 
}


void *lector(void *arg) {
    int id = (int )arg;
    while (1) {


        inicio_lect(); 
        // Leer datos
        printf("Lector %d leyendo: %d\n", id, dato);
        usleep(rand() % 1000000); // Retraso aleatorio de hasta 1 segundo (en microsegundos)
        
        //fin de leer datos
        fin_lect(); 
        usleep(rand() % 1000000); 

    }
}

void *escritor(void *arg) {
    int id = (int )arg;
     int aux;
    while (1) {

        inicio_esc(); 
        // Escribir datos
        aux = dato;
        usleep(rand() % 5000);
        aux++;
        usleep(rand() % 5000);
        dato = aux;
        usleep(rand() % 5000);
        printf("Escritor %d escribiendo: %d\n", id, dato);
        printf("Escritores esperando: %d\n", nesc_esperando); 

        usleep(rand() % 2000000); // Retraso aleatorio de hasta 2 segundos (en microsegundos)

        //fin de escribir datos
        fin_esc(); 

        usleep(rand() % 5000000);

    }
}

int main() {
    

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

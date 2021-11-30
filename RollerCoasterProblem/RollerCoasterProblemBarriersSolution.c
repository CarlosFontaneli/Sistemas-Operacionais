//incluindo as bibliotecas necessarias
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

//definindo as quantidades
#define MAX_RIDES 3
#define PASSENGER_TOTAL 11
#define CAR_CAPACITY 5

//declarando as barreiras
pthread_barrier_t loadBarrier;    //permissão para embarcar
pthread_barrier_t unloadBarrier;  //permissão para desembarcar
pthread_barrier_t boardBarrier;   //embarque dos passageiros
pthread_barrier_t unboardBarrier; //desembarque dos passageiros
sem_t boardSemaphore;             //semáforo de embarque
sem_t unboardSemaphore;           //semáforo de desembarque

volatile int ridesQuantity = 0; //quantidade de corridas, é volatil pra sincronizar

void *car_f();
void *passenger_f(void *);
void run();
void load();
void unload();
void board(int);
void unboard(int);

int main()
{
    int i;
    srandom(time(NULL)); //função de tempo aleatório que garantem tempo necessario para as ações dos passageiros ocorrerem corretamente

    //declarando as threads
    pthread_t carThread, passengersThreads[PASSENGER_TOTAL];

    //inicializando os semáforos
    sem_init(&boardSemaphore, 0, CAR_CAPACITY);
    sem_init(&unboardSemaphore, 0, CAR_CAPACITY);

    //inicializando as barreiras, com valor total+1
    pthread_barrier_init(&loadBarrier, NULL, CAR_CAPACITY + 1);
    pthread_barrier_init(&unloadBarrier, NULL, CAR_CAPACITY + 1);
    pthread_barrier_init(&boardBarrier, NULL, CAR_CAPACITY + 1);
    pthread_barrier_init(&unboardBarrier, NULL, CAR_CAPACITY + 1);

    //criando a thread do carro
    pthread_create(&carThread, NULL, &car_f, NULL);

    //criando as threads doos passageiros
    for (i = 0; i < PASSENGER_TOTAL; i++)
    {
        int *threadId = malloc(sizeof(int));
        *threadId = i;

        if (pthread_create(&passengersThreads[i], NULL, &passenger_f, (void *)threadId) != 0)
            perror("Falha ao criar a thread");
    }

    pthread_join(carThread, NULL);

    printf("\nPoxa, o parque fechou... Até a próxima pessoal!\n");

    //destruindo os semáforos e as barreiras
    sem_destroy(&boardSemaphore);
    sem_destroy(&unboardSemaphore);

    pthread_barrier_destroy(&unloadBarrier);
    pthread_barrier_destroy(&boardBarrier);
    pthread_barrier_destroy(&unboardBarrier);

    return 0;
}

//carrinho da montanha russa, que continua correndo até atingir o maximo permitido
//ele aciona as funções load() e unload(), que sinalizam a autorização do embarque/desembarque, e realiza a corrida
void *car_f()
{
    while (ridesQuantity < MAX_RIDES)
    {
        load();

        pthread_barrier_wait(&loadBarrier);
        pthread_barrier_wait(&boardBarrier);
        run();

        pthread_barrier_wait(&unloadBarrier);
        unload();
        pthread_barrier_wait(&unboardBarrier);
        ridesQuantity++;
    }
    pthread_exit(0);
}

//função dos passageiros da montanha russa, que continuam embarcando e desembarcando até o fim das corridas em um looping infinito
void *passenger_f(void *id)
{
    int threadId = *(int *)id;

    while (1)
    {
        sem_wait(&boardSemaphore);           //o semáforo para garantir que os passageiros embarquem de acordo com a capacidade do carrinho
        pthread_barrier_wait(&loadBarrier);  // barreira para as threads que solicitaram do embarque
        board(threadId);                     //inicio do embarque
        pthread_barrier_wait(&boardBarrier); // a barreira que garante o embarque de todos os passageiros

        sem_wait(&unboardSemaphore);           //o semáforo para garantir que os passageiros desembarquem de acordo com a capacidade do carrinho
        pthread_barrier_wait(&unloadBarrier);  // barreira para as threads que solicitaram do desembarque
        unboard(threadId);                     //inicio do desembarque
        pthread_barrier_wait(&unboardBarrier); // a barreira que controla o desembarque de todos os passageiros

        //reiniciando os semaforos para os proximos embarques e desembarques
        sem_post(&boardSemaphore);
        sem_post(&unboardSemaphore);
    }

    free(id);

    return NULL;
}

//função de corrida
void run()
{
    printf("\n\nA corrida começou!\n\n");
    printf("\n--------------------------------------------------\n");
    printf("\n-------------------CORRENDO!!!--------------------\n");
    printf("\n--------------------------------------------------\n\n");
}

//função de liberar o embarque
void load()
{
    printf("\nPodem entrar no carrinho e apertar os cintos! Nosso passeio já vai começar!\n");
    sleep(random() % 3);
}

//função de liberar o desembarque
void unload()
{
    printf("A corrida acabou pessoal! Podem tirar os sintos agora! Aguardem a próxima!\n\n");
    sleep(random() % 2);
}

//função dos passageiros embarcando
void board(int id)
{
    printf("O passageiro %2d acaba de embarcar!\n", id);
    sleep(random() % 3);
}

//função dos passageiros desembarcando
void unboard(int id)
{
    sleep(random() % 3);
    printf("O passageiro %2d acaba de desembarcar!\n", id);
    sleep(random() % 3);
}
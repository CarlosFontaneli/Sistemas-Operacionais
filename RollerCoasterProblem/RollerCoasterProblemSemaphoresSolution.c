#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define MAX_RIDES 3
#define PASSENGER_TOTAL 13
#define CAR_CAPACITY 8

//declarando os semáforos
sem_t boardSemaphore;      //semáforo de embarque
sem_t unboardSemaphore;    //semáforo de desembarque
sem_t fullBoardSemaphore;  //semáforo de carrinho cheio
sem_t emptyBoardSemaphore; //semáforo de carrinho vazio

//contador que define quantidade de pessoas que embarcaram no carrinho
int onBoard = 0;

void *car_f();
void *passenger_f(void *);
void run();
void load();
void unload();
void board(int);
void unboard(int);

int main()
{
    srandom(time(NULL)); //função de tempo aleatório que garantem tempo necessario para as ações dos passageiros ocorrerem corretamente

    int i;
    pthread_t carThread, passengersThreads[PASSENGER_TOTAL];

    //inicializando os semáforos com pshared=0, pois assim eles são compartilhados entre as threads de um processo e não entre processos.
    sem_init(&boardSemaphore, 0, 0);
    sem_init(&fullBoardSemaphore, 0, 0);
    sem_init(&unboardSemaphore, 0, 0);
    sem_init(&emptyBoardSemaphore, 0, 0);

    //criando a thread do carrinho
    pthread_create(&carThread, NULL, &car_f, NULL);

    //criando as threads dos passageiros
    for (i = 0; i < PASSENGER_TOTAL; i++)
    {
        int *threadId = malloc(sizeof(int));
        *threadId = i;

        if (pthread_create(&passengersThreads[i], NULL, &passenger_f, (void *)threadId) != 0)
        {
            perror("Falha ao criar a thread");
        }
    }

    //dando join nas threads
    pthread_join(carThread, NULL);

    sleep(random() % 3);
    printf("\n\nEpa!O parque fechou! Até a próxima pessoal...\n");

    //destruindo os semáforos
    sem_destroy(&boardSemaphore);
    sem_destroy(&fullBoardSemaphore);
    sem_destroy(&unboardSemaphore);
    sem_destroy(&emptyBoardSemaphore);

    return 0;
}

//função do carrinho
void *car_f()
{
    int j, i;

    for (i = 0; i < MAX_RIDES; i++) // o carrinho continua correndo até o número máximo de corridas ser atingido
    {
        //Aqui, incrementa-se o valor do semáforo de embarque até a capacidade total do carro, para que permitam o embarque do número certo de passageiros
        //Após isso, aciona-se o semáforo fullBoard, que indica que o carrinho está cheio e, portanto, nenhuma outra thread pode entrar na região crítica
        load();
        for (j = 0; j < CAR_CAPACITY; j++)
        {

            sem_post(&boardSemaphore);
        }
        sem_wait(&fullBoardSemaphore);

        //após garantir o embarque correto do número de passageiros, inicia-se a corrida
        run();

        //Nesse ponto temos uma situação análoga a anterior, em que decrementa-se até o valor do semáforo voltar ao estado nulo, para autorizar o desembarque do número correto de passageiros
        unload();
        for (j = 0; j < CAR_CAPACITY; j++)
        {
            sem_post(&unboardSemaphore);
        }
        sem_wait(&emptyBoardSemaphore);
    }

    return NULL;
}

void *passenger_f(void *id)
{
    int threadId = *(int *)id;

    //Em um looping infinito, aqui os passageiros embarcam e desembarcam do carrinho
    while (1)
    {
        sem_wait(&boardSemaphore);
        onBoard++;
        board(threadId);
        // quando a quantidade de pessoas que embarcaram, de acordo com onBoard, atinge a capacidade total
        // ativa-se o outro semáforo, de fullBoardSemaphore, para que nenhum outro passageiro possa embarcar
        if (onBoard == CAR_CAPACITY)
        {
            sem_post(&fullBoardSemaphore);
        }

        // De maneira análoga, o desembarque funciona decrementando o número de passageiros dentro do carrinho
        // quando não há mais nenhum passageiro no carrinho, ativa-se o semáforo emptyBoard, que indica que o carrinho esvaziou
        sem_wait(&unboardSemaphore);
        onBoard--;
        unboard(threadId);
        if (onBoard == 0)
        {
            sem_post(&emptyBoardSemaphore);
        }
    }

    return NULL;
}
//função de corrida
void run()
{
    sleep(random() % 3);
    printf("\n\nEba...A corrida começou!\n");
    sleep(random() % 3);
    printf("\nCorrendo...\n");
    sleep(random() % 3);
    printf("\nPoxa...A corrida acabou!\n\n");
}

//função que antecede o embarque
void load()
{
    sleep(random() % 3);
    printf("\nPodem entrar no carrinho e apertem os cintos!\n\n");
}

//função que antecede o desembarque
void unload()
{
    sleep(random() % 3);
    printf("\nPodem descer no carrinho\n\n");
}

//função de embarque
void board(int id)
{
    printf("+ Passageiro %2d embarcou\n", id);
    sleep(random() % 3);
}

//função de desembarque
void unboard(int id)
{
    printf("- Passageiro %2d desembarcou\n", id);
    sleep(random() % 3);
}
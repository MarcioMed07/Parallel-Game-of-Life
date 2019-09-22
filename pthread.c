/******************************
* Created by: Márcio Medeiros *
* in: 20/08/2019              *
******************************/

#include "header.h"


void run(int row_start, int row_end, int id) {
    while (itt < iterations) {
        core(row_start, row_end);

        // wait for all threads        
        pthread_barrier_wait(&barrier);

        // copy boards
        if (id == 0) {
            aux = lifeN;
            lifeN = life;
            life = aux;
            itt++;
        }

        // wait for all threads        
        pthread_barrier_wait(&barrier);
    }

    pthread_exit(NULL);
}

void* gameOfLife(void* arg) {
    int id = *(int*) arg; // thread id
    int row_start = id * rows_thread;
    int row_end = row_start + rows_thread;

    if (id == num_threads - 1)
        row_end += rows_last_thread;

    run(row_start, row_end, id);

    return NULL;
}


//Executes the Game of Life Algorithim showing the initial and final boards
// arg 1 = boards height
// arg 2 = boards width
// arg 3 = number of iterations
// arg 4 = file input board
// arg 5 = number of threads

int main(int argc, char **argv) {
    if (argc < 6) exit(0);
    clock_t ti, tf;

    height = atoi(argv[1]);
    width = atoi(argv[2]);
    iterations = atoi(argv[3]);
    itt = 0;
    char *file_name = argv[4];
    num_threads = atoi(argv[5]);

    pthread_t threads[num_threads];
    int flag, i;
    int *id;

    life = createBoard();
    lifeN = createBoard();
    initBoard(life, file_name);
    // showBoard(life);

    // divisão das linhas por thread
    rows_thread = height / num_threads; 
    rows_last_thread = height % num_threads; //linhas para ultima thread

    pthread_barrier_init(&barrier, NULL, num_threads);

    ti = clock();

    for (i = 0; i < num_threads; i++) {
        id = malloc(sizeof(int));
        *id = i;
        flag = pthread_create(&threads[i], NULL, gameOfLife, id);
        if (flag!=0)
            printf("Erro na criacao da thread\n");
    }

    // printf("esperando threads...\n");
    for(i = 0; i < num_threads; i++)
        pthread_join(threads[i], NULL); /*uniao das threads */

    tf = clock();
    printf("Tempo de execucao: %lf", ((double)tf-ti)/((CLOCKS_PER_SEC/1000)));

    // showBoard(life);

    pthread_barrier_destroy(&barrier);
    freeBoard(life);
    freeBoard(lifeN);
    return 0;
}
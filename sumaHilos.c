//
// Created by Said González Conde on 09/25/19.
// Suma parcial de un vector de 100 elementos con hilos
//

#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int i, a[100];

void *sumar(void *P){
    int suma = 0;
    for (i = 25 * (int)P; i < 25 * ((int)P + 1) - 1; i++){
        printf("%2d + ", a[i]);
        suma += a[i];
    }
    printf("%2d", a[25 * ((int)P + 1) - 1]);
    printf(" = %d\n\n\n", suma);

    return 0;
}

int main(){
    pthread_t thread[4];

    system("clear");
    srand(time(NULL));

    for (i = 0; i < 100; i++){
        a[i] = rand() % 100;
        printf("%2d ", a[i]);
        if (((i + 1) % 25) == 0)
            printf("\n");
    }
    printf("\n\n");

    pthread_create(&thread[0], NULL, sumar, (void *)0);
    pthread_join(thread[0], NULL);

    pthread_create(&thread[1], NULL, sumar, (void *)1);
    pthread_join(thread[1], NULL);

    pthread_create(&thread[2], NULL, sumar, (void *)2);
    pthread_join(thread[2], NULL);

    pthread_create(&thread[3], NULL, sumar, (void *)3);
    pthread_join(thread[3], NULL);

    return 0;
}
//
// Created by Said González Conde on 09/27/19.
// Matrix multiplication by columns with processes
//

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

void spin_lock_init();
void spin_lock();
void spin_unlock();

int process_fork(int nproc)
{
    int i;
    for (i = 1; i <= nproc - 1; i++)
        if (fork() == 0)
            return (i);
    return (0);
}

key_t shm_key;

int A[100][100];
int B[100][100];
int c[100][100];
int *res;
int partial;
int i, j, n, *p;
int pid, status;
int shmid, semid, pid;
int C[100];

int main(){
    srand(time(NULL));

    pid_t wpid;

    spin_lock_init(&semid);
    shm_key = 0x5675;
    shmid = shmget(shm_key, sizeof(C), (IPC_CREAT | 0600));
    res = shmat(shmid, 0, 0);

    printf("Give me the matrix size (n): ");
    scanf("%d", &n);

    printf("Matrix A:\n");

    for (size_t i = 0; i < n; i++){
        for (size_t j = 0; j < n; j++){
            A[i][j] = rand() % 9;
            printf("%3d ", A[i][j]);
        }
        printf("\n");
    }

    printf("\nMatrix B:\n");

    for (size_t i = 0; i < n; i++){
        for (size_t j = 0; j < n; j++){
            B[i][j] = rand() % 9;
            printf("%3d ", B[i][j]);
        }
        printf("\n");
    }

    pid = process_fork(n + 1);

    switch (pid){
    case 0:
        while ((wpid = wait(&status)) > 0)
            ;
        printf("\n\nFather\n");
        printf("\nMatrix C:\n");
        for (size_t i = 0; i < n * n; i++){
            printf(" %5d", res[i]);
            if ((i + 1) % n == 0)
                printf("\n");
        }

        break;

    default:

        printf("\nchild %d\n", pid);
        for (size_t i = 0; i < n; i++){
            partial = 0;
            for (size_t j = 0; j < n; j++){
                partial += A[pid - 1][j] * B[j][i];
                printf("%d*%d", A[pid - 1][j], B[j][i]);
                if (j != (n-1))
                    printf(" + ");
            }
            spin_lock(&semid);
            res[(n * (pid - 1)) + i] = partial;
            printf(" = %d\n", res[(n * (pid - 1)) + i]);
            spin_unlock(&semid);
        }
        exit(0);

        break;
    }

    return 0;
}

void spin_lock_init(int *lok)
{
    int init_sem_value = 1;
    *lok = semget(IPC_PRIVATE, 1, (0600 | IPC_CREAT));
    if (*lok == -1){
        perror("semget");
        exit(1);
    }
    if (semctl(*lok, 0, SETVAL, init_sem_value) < 0){
        perror("semctl");
        exit(1);
    }
}

void spin_lock(int *lok){
    struct sembuf sembuffer, *sops;
    sops = &sembuffer;
    sops->sem_num = 0;
    sops->sem_op = -1;
    sops->sem_flg = 0;
    if (semop(*lok, sops, 1) < 0){
        perror("semop");
        exit(1);
    }
} 

void spin_unlock(int *lok){
    struct sembuf sembuffer, *sops;
    sops = &sembuffer;
    sops->sem_num = 0;
    sops->sem_op = 1;
    sops->sem_flg = 0;
    if (semop(*lok, sops, 1) < 0){
        perror("semop");
        exit(1);
    }
}

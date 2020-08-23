#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<sys/sem.h>
//buffer for semahpore operations
struct sembuf sem_op;
pid_t fish;
pid_t pellet;
char (*a)[10];
int status;
int shmid;
int semid;
void handler(int);

int main(int argc, char *argv[]) {
	// To allow ^C
	signal(SIGINT, handler);
	// ftok to generate unique key
	key_t key = ftok("shmfile", 66);
	// shmget returns an identifier in shmid
	shmid = shmget(key, sizeof(char[10][10]), 0666 | IPC_CREAT);
	// shmat to attach to shared memory
	a = shmat(shmid, (void*) 0, 0);
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			a[i][j] = '~';
		}
	}

	//Create the semaphore.
	semid = semget(IPC_PRIVATE, 1, 0600);
	if (semid == -1) {
	    perror("semget");
	    exit(1);
	}
	//intializing the semaphore value
	semctl(semid, 0, SETVAL, 1);

	//Creating new child process
	fish = fork();

	if (fish == -1) {
		perror("Fish fork failed1");
		exit(1);
	} else if (fish == 0) {
		execl("fish", "fish", NULL);
		perror("Fish exec failed");
		exit(1);

	}
	int timer = 0;
	while (timer <= 30) { // Run for 30 seconds
		// for Decrementing the semaphore value
		sem_op.sem_num = 0;
		sem_op.sem_op = -1;
		sem_op.sem_flg = 0;
		semop(semid, &sem_op, 1);
		pellet = fork();
		if (pellet == -1) {
			perror("Pellet fork failed1");
			printf("ERROR");
			exit(1);
		} else if (pellet == 0) {
			execl("./pellet", "pellet", NULL);
			perror("pellet exec failed");
			exit(1);
		}
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				printf("%c ", a[i][j]);
			}
			printf("\n");
		}
		printf("\n");
		sleep(1);
		//For incrementing the value of semaphore
		sem_op.sem_num = 0;
		sem_op.sem_op = 1;
		sem_op.sem_flg = 0;
		semop(semid, &sem_op, 1);
		timer++;

	}
	//For killing the process
	kill(fish, SIGUSR1);
	kill(pellet, SIGUSR1);
	//Delete Shared memory
	shmdt(a);
	shmctl(shmid, IPC_RMID, NULL);
	// Delete the semaphore
	if (semctl(semid, 0, IPC_RMID, 0) != 0) {
		fprintf(stderr, "Couldn't remove the semaphore!\n");
		exit(1);
	}
	return 0;
}

void handler(int num) {
// Ctrl C exits program
	kill(fish, SIGUSR1);
	kill(pellet, SIGUSR1);
	shmdt(a);
	shmctl(shmid, IPC_RMID, NULL);
	// Delete the semaphore
	if (semctl(semid, 0, IPC_RMID, 0) != 0) {
		fprintf(stderr, "Couldn't remove the semaphore!\n");
		exit(1);
	}
	perror("Interrupt signal is pressed!! \n");
	exit(1);
}

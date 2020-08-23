#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include<errno.h>
#include<sys/sem.h>

void handler(int);
char (*a)[10];
//#define SEM_KEY		1234
//struct sembuf g_lock_sembuf[1];
//struct sembuf g_unlock_sembuf[1];
//int rtn;
int main(int argc, char *argv[]) {
        // To allow ^C
	signal(SIGINT, handler); 
	// ftok to generate unique key
	key_t key = ftok("shmfile", 66);

	// shmget returns an identifier in shmid
	int shmid = shmget(key, sizeof(char[10][10]), 0666 | IPC_CREAT);

	// shmat to attach to shared memory

	a = shmat(shmid, (void*) 0, 0);


	//To generate a random number
	srand(time(NULL)); 
	int r = rand() % 9;
	sleep(1);
	a[0][r] = 'p';
	int count = 0;
	while (1) {
		sleep(1);
		/*while ((rtn = semop(semid, g_lock_sembuf, 1)), rtn != 0
						&& errno == EAGAIN) {
					usleep(10000);
				}*/
		if (count < 9) {
			count++;
			if (a[count][r] == 'f') {
				a[count - 1][r] = '~';
				printf("Pellet Eaten\n");
				printf("PID: %d \n", getpid());
				break;
			}
			a[count][r] = 'p';
			a[count - 1][r] = '~';

		} else {
			if (a[count][r] == 'p') {
				a[count][r] = '~';
				printf("Pellet Missed \n");
				printf("PID: %d \n", getpid());
			}
			break;
		}

	/*if (semop(semid, g_unlock_sembuf, 1) == -1) {
				fprintf(stderr, "semop() call failed, could not unlock semaphore!");
				exit(1);
			}*/
	}
	//Detaching from shared memory
	shmdt(a);
	return 0;
}

void handler(int num) {

	shmdt(a);
	exit(1);
}

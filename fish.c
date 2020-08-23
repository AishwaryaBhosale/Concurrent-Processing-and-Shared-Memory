#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include<sys/sem.h>
/*
#define SEM_KEY		1234
struct sembuf g_lock_sembuf[1];
struct sembuf g_unlock_sembuf[1];*/
int rtn;
char (*a)[10];
void handler(int);

int findclosestPellet(char (*a)[10], int pos);
int main(int argc, char *argv[]) {
	// To allow ^C
	signal(SIGINT, handler);  
	// ftok to generate unique key
	key_t key = ftok("shmfile", 66);

	// shmget returns an identifier in shmid
	int shmid = shmget(key, sizeof(char[10][10]), 0666 | IPC_CREAT);
	/*g_lock_sembuf[0].sem_num = 0;
	g_lock_sembuf[0].sem_op = -1;
	g_lock_sembuf[0].sem_flg = 0;
	g_unlock_sembuf[0].sem_num = 0;
	g_unlock_sembuf[0].sem_op = 1;
	g_unlock_sembuf[0].sem_flg = 0;
	int semid;
	if ((semid = semget( SEM_KEY, 1, IPC_CREAT | 0666)) == -1) {
			fprintf(stderr, "semget() call failed, could not create semaphore!");
			exit(1);
		}*/


	// shmat to attach to shared memory
	a = shmat(shmid, (void*) 0, 0);

	a[9][4] = 'f';
	int pos = 4;
	int pellet = findclosestPellet(a, pos);
	sleep(1);
	while (1) {
		/*while ((rtn = semop(semid, g_lock_sembuf, 1)), rtn != 0
				&& errno == EAGAIN) {
			usleep(10000);
		}
*/
		if (rtn != 0) {
			fprintf(stderr, "semop() call failed, could not lock semaphore!");
			exit(1);
		}
		while (abs(pos - pellet) > 0) {
			if (pellet < pos) {
				a[9][pos - 1] = 'f';
				a[9][pos] = '~';
				pos = pos - 1;

			} else if (pellet > pos) {
				a[9][pos + 1] = 'f';
				a[9][pos] = '~';
				pos = pos + 1;
			}
			sleep(1);
		}

		pellet = findclosestPellet(a, pos);


	}

	//detach from shared memory
	/*if (semop(semid, g_unlock_sembuf, 1) == -1) {
				fprintf(stderr, "semop() call failed, could not unlock semaphore in fish!");
				exit(1);
			}*/
	shmdt(a);
	return 0;
}
int findclosestPellet(char (*a)[10], int pos) {
	int k = 0;
	int fx = 9;
	int fy = pos;
	double min = 999;
	double dist = 0;
	int col = 0;
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++)
			if (a[i][j] == 'p') {
				dist = sqrt(((fx - i) * (fx - i)) + ((fy - j) * (fy - j)));
				if (dist < min) {
					min = dist;
					col = j;
				}
			}
	}
	return col;
}

void handler(int num) {
	//Detaching from shared memory
	shmdt(a);
	exit(1);
}

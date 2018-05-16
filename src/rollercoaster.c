#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_PASSENGERS 25
#define PASSENGERS rand() % MAX_PASSENGERS + 2
#define CAPACITY rand() % PASSENGERS - 1

sem_t check_in_lock;
sem_t riding_lock;
sem_t board_queue;
sem_t all_boarded;
sem_t unboard_queue;
sem_t all_unboarded;

int boarded = 0;
int unboarded = 0; 

void load(){
	printf("A new ride will begin, time to load!\n");
	sleep(1);
}

void run(){
	printf("The car is full, time to ride!\n");
	sleep(1);
	printf("The car is now riding the roller coaster!\n");
	sleep(rand()%4);
}

void unload(){
	printf("The ride is over, time to unload!\n");
	sleep(1);
}

void board(){
	printf("%d passengers have boarded the car...\n", boarded);
	sleep(rand()%2);
}

void unboard(){
	printf("%d passengers have unboarded the car...\n", unboarded);
	sleep(rand()%2);
}

void* carThread(){
	int i;
	
	load();
	for(i = 0; i < CAPACITY; i++) sem_post(&board_queue);
	sem_wait(&all_boarded);
	run();
	unload();
	for(i = 0; i < CAPACITY; i++) sem_post(&unboard_queue);
	sem_wait(&all_unboarded);

	return NULL;
}

void* passengerThread(){
	sem_wait(&board_queue);
		
	sem_wait(&check_in_lock);
	boarded++;
	board();
	if (boarded == CAPACITY){
		sem_post(&all_boarded);
		boarded = 0;
	}
	sem_post(&check_in_lock);

	sem_wait(&unboard_queue);
	
	sem_wait(&riding_lock);
	unboarded++;
	unboard();
	if (unboarded == CAPACITY){
		sem_post(&all_unboarded);
		unboarded = 0;
	}
	sem_post(&riding_lock);

	return NULL;
}

int main() {
	pthread_t passenger[PASSENGERS];
	pthread_t car;
	int i;
	
	sem_init(&board_queue, 0, 0);
	sem_init(&all_boarded, 0, 0);
	sem_init(&check_in_lock, 0, 1);
	sem_init(&riding_lock, 0, 1);
	sem_init(&unboard_queue, 0, 0);
	sem_init(&all_unboarded, 0, 0);
	
	pthread_create(&car, NULL, carThread, NULL);
	for(i = 0; i < PASSENGERS; i++)
		pthread_create(&passenger[i], NULL, passengerThread, NULL);
	
	pthread_join(car, NULL);
	for(i = 0; i < PASSENGERS; i++)
		pthread_join(passenger[i], NULL);

	sem_destroy(&board_queue);
	sem_destroy(&all_boarded);
	sem_destroy(&check_in_lock);
	sem_destroy(&riding_lock);
	sem_destroy(&unboard_queue);
	sem_destroy(&all_unboarded);

	return 0;
}

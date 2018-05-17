#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

// Maximum number of passenger threads available
#define MAX_PASSENGERS 25

/* Variables */
pthread_mutex_t check_in_lock; // mutex to control access of the 'boarded' variable
pthread_mutex_t riding_lock; // mutex to control access of the 'unboarded' variable

sem_t board_queue; // semaphore to ensure boarding of C passenger threads
sem_t all_boarded; // binary semaphore to tell passenger threads to wait for the next ride
sem_t unboard_queue; // semaphore to ensure unboarding of C passenger threads
sem_t all_unboarded; // binary semaphore to signal passenger threads for boarding

volatile int boarded = 0; // current number of passenger threads that have boarded
volatile int unboarded = 0; // current number of passenger threads that have unboarded
volatile int current_ride = 0; // current number of rides
volatile int max_rides; // total number of coaster rides for the instance
int passengers; // current number of passenger threads
int capacity; // current capacity of the car thread

/* Helper functions */
void load(){
	printf("Ride #%d will begin, time to load!\n", current_ride+1);
	printf("This car's capacity is %d\n", capacity);
	sleep(2);
}
void run(){
	printf("The car is full, time to ride!\n");
	sleep(2);
	printf("The car is now riding the roller coaster!\n");
	sleep(5);
}
void unload(){
	printf("The ride is over, time to unload!\n");
	sleep(2);
}
void board(){
	printf("%d passengers have boarded the car...\n", boarded);
	sleep(rand()%2);
}
void unboard(){
	printf("%d passengers have unboarded the car...\n", unboarded);
	sleep(rand()%2);
}

/* Thread Functions */
void* carThread(){
	int i;
	while(current_ride < max_rides){
		load();
		
		for(i = 0; i < capacity; i++) sem_post(&board_queue); // Signal C passenger threads to board the car
		sem_wait(&all_boarded); // Tell the rest of the passengers to wait for the next ride
		
		run();
		unload();
		
		for(i = 0; i < capacity; i++) sem_post(&unboard_queue); //
		sem_wait(&all_unboarded);
		printf("The car is now empty!\n\n");
		
		current_ride++;
	}
	return NULL;
}

void* passengerThread(){
	while(1){
		sem_wait(&board_queue);
		
		pthread_mutex_lock(&check_in_lock);
		boarded++;
		board();
		if (boarded == capacity){
			sem_post(&all_boarded);
			boarded = 0;
		}
		pthread_mutex_unlock(&check_in_lock);

		sem_wait(&unboard_queue);
	
		pthread_mutex_lock(&riding_lock);
		unboarded++;
		unboard();
		if (unboarded == capacity){
			sem_post(&all_unboarded);
			unboarded = 0;
		}
		pthread_mutex_unlock(&riding_lock);
	}
    	return NULL;
}

/* Main program */
int main() {
	// Set new instance of passenger threads, car capacity and total rides values
	srand(time(NULL));
	passengers = rand() % (MAX_PASSENGERS + 2);
	capacity = rand() % (passengers - 1);
	max_rides = 1 + rand() % 5;

	pthread_t passenger[passengers];
	pthread_t car;
	int i;
	
	// Create new mutexes and semaphores
	pthread_mutex_init(&check_in_lock, NULL);
	pthread_mutex_init(&riding_lock, NULL);
	sem_init(&board_queue, 0, 0);
	sem_init(&all_boarded, 0, 0);
	sem_init(&unboard_queue, 0, 0);
	sem_init(&all_unboarded, 0, 0);

	printf("Today the roller coaster will ride %d times!\n", max_rides);
	printf("There are %d passengers waiting in the roller coaster queue!\n\n", passengers);
	
	// Create the threads and start the roller coaster
	pthread_create(&car, NULL, carThread, NULL);
	for(i = 0; i < passengers; i++) pthread_create(&passenger[i], NULL, passengerThread, NULL);
	// Join the car thread when all rides have been completed
	pthread_join(car, NULL);
	
	printf("That's all rides for today, the roller coaster is shutting down.\n");

	// Destroy mutexes and semaphores
	pthread_mutex_destroy(&check_in_lock);
	pthread_mutex_destroy(&riding_lock);
	sem_destroy(&board_queue);
	sem_destroy(&all_boarded);
	sem_destroy(&unboard_queue);
	sem_destroy(&all_unboarded);

	return 0;
}

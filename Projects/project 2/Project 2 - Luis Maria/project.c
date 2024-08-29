#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 5
#define NUM_ROOMS   3

sem_t in_res_ready; // starts as 1
sem_t out_res_ready;
sem_t at_in_desk; // starts as 0
sem_t at_out_desk;
sem_t guest_has_keys; //starts as 0
sem_t guest_has_receipt;
sem_t check_in_done; //starts as 0
sem_t check_out_done;
sem_t open_rooms; //starts as 3
uint8_t check_in_id;
uint8_t check_out_id;
uint8_t check_in_room_num;
uint8_t check_out_room_num;
uint8_t total_guests = 0;
uint8_t total_pool = 0;
uint8_t total_restaurant = 0;
uint8_t total_fitness = 0;
uint8_t total_business = 0;


//array of rooms
bool room_is_free[NUM_ROOMS] = {true, true, true};


//each activity corresponds to an integer
enum activities {
    POOL = 0,
    RESTAURANT = 1,
    FITNESS = 2,
    BUSINESS = 3
};

//struct that gets passed to the guest thread
struct guest_stuff {
    uint8_t guest_id;
    uint8_t guest_room_num;
};


//guest thread
void *guest(void *threadid) {
    struct guest_stuff *stuff = (struct guest_stuff *) threadid;

    //wait for res to be ready
    sem_wait(&in_res_ready);

    //go to front desk
    check_in_id = stuff->guest_id;
    printf("\nGuest %u goes to the check-in reservationist\n", check_in_id);
    sem_post(&at_in_desk);
    sem_wait(&check_in_done);

    //take keys
    stuff->guest_room_num = check_in_room_num;
    printf("\nGuest %u recieves room %u and completes check-in\n", check_in_id, check_in_room_num);
    sem_post(&guest_has_keys);

    //do stuff
    enum activities activity = rand() % 4;
    switch (activity) {
        case POOL:
            printf("\nGuest %u goes to the swimming pool\n", stuff->guest_id);
            total_pool += 1;
            break;
        
        case RESTAURANT:
            printf("\nGuest %u goes to the restaurant\n", stuff->guest_id);
            total_restaurant +=1;
            break;
        
        case FITNESS:
            printf("\nGuest %u goes to the fitness center\n", stuff->guest_id);
            total_fitness += 1;
            break;

        case BUSINESS:
            printf("\nGuest %u goes to the business center\n", stuff->guest_id);
            total_business += 1;
            break;

        //this should not happen
        default:
            break;
    }

    //sleep for 1 to 3 seconds
    sleep((rand() % 3) + 1);

    //wait for out_res to be ready
    sem_wait(&out_res_ready);

    //go to front desk and return the room
    check_out_id = stuff->guest_id;
    check_out_room_num = stuff->guest_room_num;
    printf("\nGuest %u goes to the check-out reservationist and returns room %u\n", check_out_id, check_out_room_num);

    sem_post(&at_out_desk);
    sem_wait(&check_out_done);

    printf("\nGuest %u receives the receipt\n", check_out_id);

    sem_post(&guest_has_receipt);
    pthread_exit(NULL);
}

//check-in reservationist thread
void *check_in_res(void *threadid) {
    uint8_t checked_in_guests = 0;
    while(checked_in_guests < NUM_THREADS) {
        sem_wait(&at_in_desk);

        //greet guest
        printf("\nThe check-in reservationist greets Guest %u\n", check_in_id);

        sem_wait(&open_rooms);

        //room should now be open
        checked_in_guests += 1;        
        total_guests += 1;
        
        //assign room
        for (uint8_t i=0; i<NUM_ROOMS; i++) {
            if (room_is_free[i]) {
                room_is_free[i] = false;
                check_in_room_num = i;
                break;
            }
        }
        printf("\nCheck-in reservationist assigns room %u to Guest %u\n", check_in_room_num, check_in_id);
        sem_post(&check_in_done);
        sem_wait(&guest_has_keys);
        sem_post(&in_res_ready);
    }
    pthread_exit(NULL);
}

//check-out reservationist thread
void *check_out_res(void *threadid) {
    uint8_t checked_out_guests = 0;
    while(checked_out_guests < NUM_THREADS) {
        sem_wait(&at_out_desk);
        
        //greet guest
        printf("\nThe check-out reservationist greets Guest %u and receives the key from room %u\n", check_out_id, check_out_room_num);

        //checkout
        room_is_free[check_out_room_num] = true;
        checked_out_guests += 1;
        sem_post(&open_rooms);

        printf("\nGuest %u's receipt was printed\n", check_out_id);
        sem_post(&check_out_done);
        sem_wait(&guest_has_receipt);
        sem_post(&out_res_ready);
    }

    printf("\nTotal guests: %u"
    "\nPool: %u"
    "\nRestaurant: %u"
    "\nFitness Center: %u"
    "\nBusiness Center: %u\n", 
    total_guests,
    total_pool,
    total_restaurant,
    total_fitness,
    total_business);

    pthread_exit(NULL);
}

//main routine
int main () {
    srand(time(NULL));
    sem_init(&at_in_desk, 0, 0);
    sem_init(&at_out_desk, 0 , 0);
    sem_init(&open_rooms, 0, 3);
    sem_init(&guest_has_keys, 0 , 0);
    sem_init(&guest_has_receipt, 0, 0);
    sem_init(&check_in_done, 0 , 0);
    sem_init(&check_out_done, 0, 0);
    sem_init(&in_res_ready, 0, 1);
    sem_init(&out_res_ready, 0 , 1);

    pthread_t guests[NUM_THREADS];
    pthread_t check_in;
    int check_in_id = 5;
    pthread_t check_out;
    int check_out_id = 6;
    pthread_t guest_threads[5];
    struct guest_stuff stuff[5];

    int check_in_ret = pthread_create(&check_in, NULL, check_in_res, (void *)&check_in_id);
    int check_out_ret = pthread_create(&check_out, NULL, check_out_res, (void *)&check_out_id);

    for (uint8_t i = 0; i< NUM_THREADS; i++) {
        stuff[i].guest_id = i;
        pthread_create(&guest_threads[i], NULL, guest, (void *)&stuff[i]);
    }

    for (uint8_t i = 0; i< NUM_THREADS; i++) {
        pthread_join(guest_threads[i], NULL);
    }
    pthread_join(check_in, NULL);
    pthread_join(check_out, NULL);
}
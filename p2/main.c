#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include "queue.h"

// initializing time global variables
static struct timeval start_time;
double start_sec;
double b_waiting_time;
double e_waiting_time;
double overall_waiting_time;

// initializing queue global variables
Node *head_b = NULL;
Node *head_e = NULL;

// initializing pthread mutex global variables
pthread_mutex_t customer_queue;
pthread_mutex_t customer_waiting;
pthread_mutex_t clerk_waiting;
pthread_mutex_t editing_customer_done;
pthread_mutex_t clerk_isEmpty;

// initializing pthread conditional global variables
pthread_cond_t signal_customers;
pthread_cond_t signal_clerk;

// initializing global variables
int TOTAL_CUSTOMERS;
int customer_at_clerk;
int customer_done;
pthread_t clerk_id;
bool queue_in_progress = false;
bool clerk_in_progress = false;
int num_buisness = 0;
int num_economy = 0;

// initializing customer structure
typedef struct Customer
{
    int user_id;
    int class_type;
    int arrival_time;
    int service_time;

} Customer;

// function declarations
void customers_thread(Customer *customer);
void clerk_thread();
int str_to_int(char str[]);

int main(int argc, char **argv)
{
    // getting the start time of the simulation
    gettimeofday(&start_time, NULL);
    start_sec = (start_time.tv_sec + (double)start_time.tv_usec / 1000000);

    // initializing mutex and conditional variables
    pthread_mutex_init(&customer_queue, NULL);
    pthread_mutex_init(&customer_waiting, NULL);
    pthread_mutex_init(&clerk_waiting, NULL);
    pthread_mutex_init(&editing_customer_done, NULL);
    pthread_mutex_init(&clerk_isEmpty, NULL);
    pthread_cond_init(&signal_customers, NULL);
    pthread_cond_init(&signal_clerk, NULL);

    // opening file to get number of customers
    char filename[100];
    strcpy(filename, argv[1]);
    FILE *file = fopen(filename, "r");
    char holder[100];
    fgets(holder, 100, file); //gets number of customers
    TOTAL_CUSTOMERS = str_to_int(holder);

    // declaring pthreads
    pthread_t customers[TOTAL_CUSTOMERS];
    pthread_t clerks[4];

    // creating customer threads
    int i = 0;
    while (i < TOTAL_CUSTOMERS)
    {
        int user_id;
        int class_type;
        int arrival_time;
        int service_time;

        fscanf(file, "%d:%d,%d,%d", &user_id, &class_type, &arrival_time, &service_time);

        Customer new_customer = {user_id, class_type, arrival_time, service_time};
        Customer *temp = malloc(sizeof(Customer));
        *temp = new_customer;

        if (pthread_create(&customers[i], NULL, &customers_thread, temp) != 0)
        {
            printf("Failed to create thread for %d th customer.", i);
        }

        i++;
    }

    fclose(file);

    // initializing clerk threads
    for (int j = 0; j < 5; j++)
    {
        if (pthread_create(&clerks[j], NULL, &clerk_thread, NULL) != 0)
        {
            printf("Failed to create clerk.");
        }
    }

    // ensuring that all customer threads terminate before finishing
    for (int k = 0; k < TOTAL_CUSTOMERS; k++)
    {
        if (pthread_join(customers[k], NULL) != 0)
        {
            printf("Failed to wait for customer %d th thread.", k);
        }
    }

    // printing average waiting times
    printf("The average waiting time for all customers in the system is: %.2f seconds.\n", overall_waiting_time / TOTAL_CUSTOMERS);
    printf("The average waiting time for buisness-class customers is: %.2f seconds.\n", b_waiting_time / num_buisness);
    printf("The average waiting time for economy-class customers is: %.2f seconds.\n", e_waiting_time / num_economy);

    // destroying mutex and conditional variables
    pthread_mutex_destroy(&customer_queue);
    pthread_mutex_destroy(&customer_waiting);
    pthread_mutex_destroy(&clerk_waiting);
    pthread_mutex_destroy(&editing_customer_done);
    pthread_mutex_destroy(&clerk_isEmpty);
    pthread_cond_destroy(&signal_customers);
    pthread_cond_destroy(&signal_clerk);
}

/*---------------------------------------------------------------------------------------*/

/*
    Function: customer_thread
    Description: 
        - checks to see if the customer's arrival and service times are legal values
        - waits the duration of the customer's arrival time
        - adds the customer to it's respective queue based on its class (buisness or economy)
        - waits to get signalled that a clerk is servicing it
        - adds the customer's waiting time to the overall waiting time and the class waiting time
        - waits the duration of the customer's service time
        - signals to the clerk that it is done
    Paramerters: 
        - customer: a struct defining a customer
*/
void customers_thread(Customer *customer)
{
    // checking to see if the arrival and service times are legal values
    if (customer->arrival_time < 0 || customer->service_time < 0)
    {
        printf("Customer %d was given illegal values.\n", customer->user_id);
        free(customer);
        pthread_exit(NULL);
    }

    struct timeval init_time, clerk_time, end_time;
    double init_sec, clerk_sec, end_sec;

    // waiting for arrival of customemr
    usleep(customer->arrival_time * 100000);

    // getting and printing time when the customer arrives
    gettimeofday(&init_time, NULL);
    init_sec = (init_time.tv_sec + (double)init_time.tv_usec / 1000000);
    printf("A customer arrives: customer ID %2d.\n", customer->user_id);

    // adding customer to either buisness or economy queue
    pthread_mutex_lock(&customer_queue);

    if (customer->class_type == 1)
    {

        head_b = enqueue(head_b, customer->user_id);
        num_buisness++;
        int length = getLength(head_b);
        printf("A customer enters a queue: the queue ID %1d, and length of the queue %2d.\n", &head_b, length);
    }
    else
    {
        head_e = enqueue(head_e, customer->user_id);
        num_economy++;
        int length = getLength(head_e);
        printf("A customer enters a queue: the queue ID %1d, and length of the queue %2d.\n", &head_e, length);
    }

    pthread_mutex_unlock(&customer_queue);

    // waits for a clerk to signal that it is being serviced
    while (customer->user_id != customer_at_clerk)
    {
        pthread_cond_wait(&signal_customers, &customer_waiting);
    }

    pthread_t clerk = clerk_id;
    queue_in_progress = false;

    // getting the time that the clerk starts servicing it
    gettimeofday(&clerk_time, NULL);
    clerk_sec = (clerk_time.tv_sec + (double)clerk_time.tv_usec / 1000000);

    // adding waiting time to the overall waiting time and the class waiting time
    if (customer->class_type == 1)
    {
        b_waiting_time += clerk_sec - init_sec;
        overall_waiting_time += clerk_sec - init_sec;
    }
    else
    {
        e_waiting_time += clerk_sec - init_sec;
        overall_waiting_time += clerk_sec - init_sec;
    }

    pthread_mutex_unlock(&customer_waiting);

    printf("A clerk starts serving a customer: start time %.2f, the customer ID %2d, the clerk ID %1d.\n", clerk_sec - start_sec, customer->user_id, clerk);

    // sleeping for service time
    usleep(customer->service_time * 100000);

    gettimeofday(&end_time, NULL);
    end_sec = (end_time.tv_sec + (double)end_time.tv_usec / 1000000);

    printf("A clerk finishes serving a customer: end time %.2f, the customer ID %2d, the clerk ID %1d.\n", end_sec - start_sec, customer->user_id, clerk);

    pthread_mutex_lock(&editing_customer_done);

    // signaling to clerk that it is done
    while (clerk_in_progress != false)
    {
        usleep(1 * 1000000);
    }

    clerk_in_progress = true;

    customer_done = customer->user_id;
    pthread_cond_broadcast(&signal_clerk);

    pthread_mutex_unlock(&editing_customer_done);

    free(customer);
}

/*---------------------------------------------------------------------------------------*/

/*
    Function: clerk_thread
    Description: 
        - checks to see if either of the linked lists have customers
        - dequeue's the customer and stores the customer's user_id in a global variable
        - broadcasts to all the customers that it is servicing one of them
        - waits for a customer to be done servicing
    Parameters: None
*/
void clerk_thread()
{
    while (1)
    {
        // checking to see if a queue is empty
        pthread_mutex_lock(&clerk_isEmpty);
        while (isEmpty(head_b) && isEmpty(head_e))
        {
            usleep(1 * 1000000);
        }

        int id;

        while (queue_in_progress != false)
        {
            usleep(1 * 1000000);
        }

        pthread_mutex_lock(&customer_queue);

        // get the user_id and dequeue the head
        if (!isEmpty(head_b))
        {
            id = head_b->id;
            head_b = dequeue(head_b);
        }
        else
        {
            id = head_e->id;
            head_e = dequeue(head_e);
        }

        pthread_mutex_unlock(&customer_queue);

        // signaling customers
        customer_at_clerk = id;
        clerk_id = pthread_self();
        queue_in_progress = true;
        pthread_cond_broadcast(&signal_customers);

        pthread_mutex_unlock(&clerk_isEmpty);

        // waiting for customer to be done servicing
        while (customer_done != id)
        {
            pthread_cond_wait(&signal_clerk, &clerk_waiting);
        }

        clerk_in_progress = false;

        pthread_mutex_unlock(&clerk_waiting);
    }
}

/*---------------------------------------------------------------------------------------*/

/*
  Function: str_to_int
  Description: 
    Takes a string of numbers and turns it into an int
  Parameters: 
    - str: a string of numbers
*/
int str_to_int(char str[])
{
    int x = atoi(str);
    return x;
}
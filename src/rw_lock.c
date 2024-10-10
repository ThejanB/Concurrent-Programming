#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include "linked_list.h"

#define MAX_RANDOM 65535

/* Function prototypes */
int calculate_sample_size(double std_dev, double mean);
void* thread_work(void* rank);
void save_results_to_file(const char* filename, float mMember, float mInsert, float mDelete, int threadCount, double mean, double stdDev);


/* Shared variables */
struct list_node_s* head = NULL; // Shared linked list
pthread_rwlock_t rwlock;         // Read-write lock for the entire list
int n = 1000;                    // Initial number of elements
int m = 10000;                   // Total number of operations
int thread_count;                // Number of threads
int mMember, mInsert, mDelete;   // Number of operations of each type
int member_count = 0, insert_count = 0, delete_count = 0; // Shared counters

/* Function to perform thread work */
void* thread_work(void* rank) {
    int total_ops = mMember + mInsert + mDelete;
    int ops_per_thread = total_ops / thread_count;
    int my_start = ((long)rank) * ops_per_thread;
    int my_end = my_start + ops_per_thread;

    if ((long)rank == thread_count - 1) my_end = total_ops; // Last thread does the rest

    unsigned int seed = time(NULL) ^ (long)rank; // Seed for rand_r

    for (int i = my_start; i < my_end; i++) {
        float prob = (float)rand_r(&seed) / RAND_MAX;
        int val = rand_r(&seed) % MAX_RANDOM;

        if (prob < (float)mMember / total_ops) {
            pthread_rwlock_rdlock(&rwlock);
            Member(val, head);
            member_count++;
            pthread_rwlock_unlock(&rwlock);
        } else if (prob < (float)(mMember + mInsert) / total_ops) {
            pthread_rwlock_wrlock(&rwlock);
            Insert(val, &head);
            insert_count++;
            pthread_rwlock_unlock(&rwlock);

        } else {
            pthread_rwlock_wrlock(&rwlock);
            Delete(val, &head);
            delete_count++;
            pthread_rwlock_unlock(&rwlock);
        }
    }

    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <thread_count> <mMember> <mInsert> <mDelete>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    thread_count = atoi(argv[1]);
    float mMember_fraction = atof(argv[2]);
    float mInsert_fraction = atof(argv[3]);
    float mDelete_fraction = atof(argv[4]);

    float sum_probabilities = mMember_fraction + mInsert_fraction + mDelete_fraction;
    if (fabs(sum_probabilities - 1.0) > 0.0001) {
        fprintf(stderr, "Probabilities do not add up to 1.0\n");
        exit(EXIT_FAILURE);
    }

    /* Calculate the number of operations of each type */
    mMember = mMember_fraction * m;
    mInsert = mInsert_fraction * m;
    mDelete = mDelete_fraction * m;
    int total_operations = mMember + mInsert + mDelete;
    if (total_operations < m) {
        mMember += (m - total_operations);
    }

    srand(time(NULL));
    clock_t start, finish;
    double elapsed;

    /* Initialize the linked list with n unique random values */
    int count = 0;
    while (count < n) {
        int val = rand() % MAX_RANDOM;
        if (Insert(val, &head)) {
            count++;
        }
    }

    pthread_rwlock_init(&rwlock, NULL);

    /* Timing variables */
    double total_time = 0.0, mean, std_dev, variance;
    int initial_runs = 10;
    double* times = malloc(initial_runs * sizeof(double));
    if (times == NULL) {
        fprintf(stderr, "Failed to allocate memory for times array\n");
        exit(EXIT_FAILURE);
    }

    /* Initial runs */
    fprintf(stderr, "Running %d initial runs to estimate mean and standard deviation to calculate required sample size\n", initial_runs);
    for (int run = 0; run < initial_runs; run++) {
        pthread_t* thread_handles = malloc(thread_count * sizeof(pthread_t));
        if (thread_handles == NULL) {
            fprintf(stderr, "Failed to allocate memory for thread handles\n");
            exit(EXIT_FAILURE);
        }

        member_count = insert_count = delete_count = 0;

        /* Create and join threads */
        start = clock();
        for (long thread = 0; thread < thread_count; thread++) {
            pthread_create(&thread_handles[thread], NULL, thread_work, (void*)thread);
        }
        for (long thread = 0; thread < thread_count; thread++) {
            pthread_join(thread_handles[thread], NULL);
        }
        finish = clock();
        elapsed = ((double)(finish - start))/ CLOCKS_PER_SEC * 1000000; // Convert to microseconds
        times[run] = elapsed;
        total_time += elapsed;

        free(thread_handles);
    }

    /* Compute mean and standard deviation */
    mean = total_time / initial_runs;
    variance = 0.0;
    for (int i = 0; i < initial_runs; i++) {
        variance += (times[i] - mean) * (times[i] - mean);
    }
    variance = variance / (initial_runs - 1);
    std_dev = sqrt(variance);
    fprintf(stderr, "Initial Mean Execution Time: %f microseconds | Initial Standard Deviation: %f microseconds\n", mean, std_dev);

    /* Calculate required sample size */
    int required_runs = calculate_sample_size(std_dev, mean);
    fprintf(stderr, "Required Samplee Size: %d\n", required_runs);
    if (required_runs < initial_runs) required_runs = initial_runs;

    /* Additional runs if needed */
    if (required_runs > initial_runs) {
        times = realloc(times, required_runs * sizeof(double));
        if (times == NULL) {
            fprintf(stderr, "Failed to reallocate memory for times array\n");
            exit(EXIT_FAILURE);
        }

        for (int run = initial_runs; run < required_runs; run++) {
            pthread_t* thread_handles = malloc(thread_count * sizeof(pthread_t));
            if (thread_handles == NULL) {
                fprintf(stderr, "Failed to allocate memory for thread handles\n");
                exit(EXIT_FAILURE);
            }

            member_count = insert_count = delete_count = 0;

            start = clock();

            for (long thread = 0; thread < thread_count; thread++) {
                pthread_create(&thread_handles[thread], NULL, thread_work, (void*)thread);
            }

            for (long thread = 0; thread < thread_count; thread++) {
                pthread_join(thread_handles[thread], NULL);
            }

            finish = clock();
            elapsed = ((double)(finish - start))/ CLOCKS_PER_SEC * 1000000; // Convert to microseconds
            times[run] = elapsed;
            total_time += elapsed;

            free(thread_handles);
        }

        /* Recompute mean and standard deviation */
        mean = total_time / required_runs;
        variance = 0.0;
        for (int i = 0; i < required_runs; i++) {
            variance += (times[i] - mean) * (times[i] - mean);
        }
        variance = variance / (required_runs - 1);
        std_dev = sqrt(variance);
    }

    printf("Final Mean Execution Time: %f microseconds | Final Standard Deviation: %f microseconds\n", mean, std_dev);
    printf("Total Samples: %d\n", required_runs);

    save_results_to_file("results/rw_lock_results.txt", mMember_fraction, mInsert_fraction, mDelete_fraction, thread_count, mean, std_dev);

    pthread_rwlock_destroy(&rwlock);
    Free_list(&head);
    free(times);

    return 0;
}

/* Function to calculate required sample size with 95% confidence level */
int calculate_sample_size(double std_dev, double mean) {
    double z_score = 1.96; // 95% confidence level
    double relative_error = 0.05; // 5% relative error
    double e = relative_error * mean; // Absolute error
    int n = (int)ceil((z_score * std_dev / e) * (z_score * std_dev / e));
    if (n < 30) n = 30;  // Ensure minimum sample size
    return n;
}

/* Function to save results to a file */
void save_results_to_file(const char* filename, float mMember, float mInsert, float mDelete, int threadCount, double mean, double stdDev) {
    FILE* fp = fopen(filename, "a");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open file %s for writing\n", filename);
        return;
    }
    fprintf(fp, "n: %d | m: %d | mMember: %f%% | mInsert: %f%% | mDelete: %f%% | threadCount: %d | mean: %f | stdDev: %f\n", n, m, mMember, mInsert, mDelete, threadCount, mean, stdDev);
    fclose(fp);
}
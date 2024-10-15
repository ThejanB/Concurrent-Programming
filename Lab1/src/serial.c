#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "linked_list.h"

#define MAX_RANDOM 65535

/* Function prototypes */
int calculate_sample_size(double std_dev, double mean);
double run_operations(float mMember, float mInsert, float mDelete, int n, int m);
void save_results_to_file(const char* filename, float mMember, float mInsert, float mDelete, int threadCount, double mean, double stdDev);

// Global variables
int n = 1000;   // Initial number of elements
int m = 10000;  // Total number of operations

int main(int argc, char* argv[]) {
    
    if (argc != 4) {
        fprintf(stderr, "Invalid number of input arguments\n");
        fprintf(stderr, "Usage: %s <mMember> <mInsert> <mDelete>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    float mMember = atof(argv[1]);
    float mInsert = atof(argv[2]);
    float mDelete = atof(argv[3]);

    /* Validate input probabilities */
    if (fabs(mMember + mInsert + mDelete - 1.0) > 0.0001) {
        fprintf(stderr, "Probabilities do not add up to 1.0\n");
        exit(EXIT_FAILURE);
    }


    int initial_runs = 100;  // Initial number of runs
    double *times; // Array to store execution times
    double sum = 0.0, mean, std_dev, variance;

    /* Seed the random number generator */
    srand(time(NULL));

    /* Allocate memory for timing array */
    times = (double *)malloc(initial_runs * sizeof(double));
    if (times == NULL) {
        fprintf(stderr, "Failed to allocate memory for times array\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Running %d initial runs to estimate mean and standard deviation to calculate required sample size\n", initial_runs);

    /* Initial serial runs to estimate mean and std_dev */
    double elapsed;
    for (int i = 0; i < initial_runs; i++) {
        elapsed = run_operations(mMember, mInsert, mDelete, n, m);
        times[i] = elapsed;
        sum += elapsed;
    }

    /* Compute mean and standard deviation */
    mean = sum / initial_runs;
    variance = 0.0;
    for (int i = 0; i < initial_runs; i++) {
        variance += (times[i] - mean) * (times[i] - mean);
    }
    variance = variance / initial_runs;
    std_dev = sqrt(variance);

    fprintf(stderr, "Initial Mean Execution Time: %f microseconds | Initial Standard Deviation: %f microseconds\n", mean, std_dev);

    /* Calculate required sample size */
    int required_runs = calculate_sample_size(std_dev, mean);
    fprintf(stderr, "Required Sample Size: %d\n", required_runs);

    /* Check if more runs are needed */
    if (required_runs > initial_runs) {
        times = (double *)realloc(times, required_runs * sizeof(double));
        if (times == NULL) {
            fprintf(stderr, "Failed to reallocate memory for times array\n");
            exit(EXIT_FAILURE);
        }
        /* Perform additional runs */
        for (int i = initial_runs; i < required_runs; i++) {
            elapsed = run_operations(mMember, mInsert, mDelete, n, m);
            times[i] = elapsed;
            sum += elapsed;
        }

        /* Recompute mean and standard deviation with all samples */
        mean = sum / required_runs;

        variance = 0.0;
        for (int i = 0; i < required_runs; i++) {
            variance += (times[i] - mean) * (times[i] - mean);
        }
        variance = variance / required_runs;
        std_dev = sqrt(variance);
    } else {
        required_runs = initial_runs;
    }
    
    printf("Final Mean Execution Time: %f microseconds | Final Standard Deviation: %f microseconds\n", mean, std_dev);
    printf("Total Samples: %d\n\n", required_runs);

    /* Clean up */
    free(times);

    /* Save results to file */
    save_results_to_file("results/serial_results.txt", mMember, mInsert, mDelete, 1, mean, std_dev);

    return 0;
}

/* Function to calculate required sample size with 95% confident level*/
int calculate_sample_size(double std_dev, double mean) {
    double z_score = 1.96; // 95% confidence level
    double e = 0.05 * mean; // Relative error
    return (int)ceil((z_score * std_dev / e) * (z_score * std_dev / e));
}

/* Function to perform m operations and return the elapsed time in microseconds */
double run_operations(float mMember, float mInsert, float mDelete, int n, int m) {
    struct list_node_s* head = NULL;
    int i;
    int total_member = 0, total_insert = 0, total_delete = 0;
    int member_count = 0, insert_count = 0, delete_count = 0;
    double elapsed;
    clock_t start, finish;

    /* Populate the linked list with n unique random values */
    int count = 0;
    while (count < n) {
        int val = rand() % MAX_RANDOM;
        if (Insert(val, &head)) {
            count++;
        }
    }

    /* Calculate the number of operations of each type */
    total_member = mMember * m;
    total_insert = mInsert * m;
    total_delete = mDelete * m;
    int total_operations = total_member + total_insert + total_delete;

    /* Adjust the number of Member operations if needed */
    if (total_operations < m) {
        total_member += (m - total_operations);
    }

    start = clock();

    for (int i = 0; i < m; i++) {
        float prob = (float)rand() / RAND_MAX;
        int val = rand() % MAX_RANDOM;

        if (prob < mMember && member_count < total_member) {
            Member(val, head);
            member_count++;
        } else if (prob < mMember + mInsert && insert_count < total_insert) {
            Insert(val, &head);
            insert_count++;
        } else if (delete_count < total_delete) {
            Delete(val, &head);
            delete_count++;
        } else if (member_count < total_member) {
            Member(val, head);
            member_count++;
        } else if (insert_count < total_insert) {
            Insert(val, &head);
            insert_count++;
        }
    }

    finish = clock();
    elapsed = ((double)(finish - start))/ CLOCKS_PER_SEC;

    Free_list(&head);

    return elapsed*1000000; // Convert to microseconds
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


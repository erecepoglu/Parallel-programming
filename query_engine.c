#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>

#define ROWS 20000000  // 20 Million Database Records for heavy computational scaling

// Function Prototypes
void generate_columnar_database(int *age_col, float *balance_col);
long long run_sequential_query(int *age_col, float *balance_col, double *exec_time);
long long run_parallel_query(int *age_col, float *balance_col, int num_threads, int schedule_type, double *exec_time);

int main() {
    // Dynamically allocate continuous heap memory for database columns
    int *age_col = (int *)malloc(ROWS * sizeof(int));
    float *balance_col = (float *)malloc(ROWS * sizeof(float));

    if (age_col == NULL || balance_col == NULL) {
        printf("Memory allocation failed! Reduce the ROWS definition.\n");
        return -1;
    }

    printf("===============================================================\n");
    printf("   Parallel Columnar Query Engine Analytics Simulator          \n");
    printf("   Database Size: %d Rows (Stored Column-Oriented)\n", ROWS);
    printf("   Query: SELECT COUNT(*) FROM Users WHERE age >= 30 AND balance > 50000\n");
    printf("===============================================================\n\n");

    printf("Populating database arrays with structured skewed load... ");
    fflush(stdout);
    generate_columnar_database(age_col, balance_col);
    printf("Done!\n\n");

    // --------------------------------------------------------------------
    // 1. SEQUENTIAL SCAN (Baseline Requirement)
    // --------------------------------------------------------------------
    double seq_time = 0.0;
    long long seq_matches = run_sequential_query(age_col, balance_col, &seq_time);
    printf(">>> Sequential Scan Finished.\n");
    printf("    Matches Found: %lld\n", seq_matches);
    printf("    Execution Time: %.4f seconds\n\n", seq_time);

    // --------------------------------------------------------------------
    // 2. PARALLEL THREAD SCALABILITY EXPERIMENTS
    // --------------------------------------------------------------------
    int thread_settings[] = {2, 4, 8, 16};
    int num_settings = sizeof(thread_settings) / sizeof(int);

    printf("-------------------------------------------------------------------\n");
    printf(" Running Thread Scalability Analysis (Using Schedule: STATIC)\n");
    printf("-------------------------------------------------------------------\n");
    printf("%-10s %-15s %-12s %-12s %-12s\n", "Threads", "Time (sec)", "Speedup", "Efficiency", "Status");

    // === ADDED: Hardcoded Sequential Baseline Row ===
    printf("%-10s %-15.4f %-12.2f %-12.2f %-12s\n", "1 (Seq)", seq_time, 1.00, 1.00, "BASELINE");

    for (int i = 0; i < num_settings; i++) {
        int threads = thread_settings[i];
        double par_time = 0.0;
        
        // Schedule type: 1 = Static
        long long par_matches = run_parallel_query(age_col, balance_col, threads, 1, &par_time);
        
        double speedup = seq_time / par_time;
        double efficiency = speedup / threads;
        int verified = (par_matches == seq_matches);

        printf("%-10d %-15.4f %-12.2f %-12.2f %-12s\n", 
               threads, par_time, speedup, efficiency, verified ? "VERIFIED" : "FAIL");
    }

    // --------------------------------------------------------------------
    // 3. SCHEDULING TYPE EXPERIMENTS (For Your Required Scheduling Graph)
    // --------------------------------------------------------------------
    printf("\n-------------------------------------------------------------------\n");
    printf(" Running OpenMP Workload Scheduling Type Analysis (Fixed at 8 Threads)\n");
    printf("-------------------------------------------------------------------\n");
    printf("%-20s %-15s\n", "Scheduling Type", "Time (sec)");

    const char *sch_names[] = {"Static", "Dynamic", "Guided"};
    for (int sch = 1; sch <= 3; sch++) {
        double sch_time = 0.0;
        run_parallel_query(age_col, balance_col, 8, sch, &sch_time);
        printf("%-20s %-15.4f\n", sch_names[sch-1], sch_time);
    }
    printf("-------------------------------------------------------------------\n");

    free(age_col);
    free(balance_col);
    return 0;
}

// Generates an intentional Load-Imbalanced Columnar Database Dataset
void generate_columnar_database(int *age_col, float *balance_col) {
    srand(100);
    for (int i = 0; i < ROWS; i++) {
        // FIRST HALF: Demographics do NOT match query (Fast execution branch)
        if (i < ROWS / 2) {
            age_col[i] = 18 + (rand() % 11);        // Age 18-28
            balance_col[i] = (float)(rand() % 30000); // Balance under 30k
        } 
        // SECOND HALF: Matches parameters (Triggers heavy processing payload)
        else {
            age_col[i] = 30 + (rand() % 45);                 // Age 30-75
            balance_col[i] = 50001.0f + (float)(rand() % 100000); // Balance over 50k
        }
    }
}

// Baseline Sequential Database Engine Filter Scan
long long run_sequential_query(int *age_col, float *balance_col, double *exec_time) {
    long long match_count = 0;
    double start = omp_get_wtime();

    for (int i = 0; i < ROWS; i++) {
        if (age_col[i] >= 30 && balance_col[i] > 50000.0f) {
            // Simulated Data Row Unpacking / Computationally Intense Filter Load
            double processing_load = 0.0;
            for (int dummy = 0; dummy < 60; dummy++) {
                processing_load += sin(dummy) * cos(dummy);
            }
            match_count++;
        }
    }
    *exec_time = omp_get_wtime() - start;
    return match_count;
}

// Highly Configurable Parallel OpenMP Columnar Query Scan Engine
long long run_parallel_query(int *age_col, float *balance_col, int num_threads, int schedule_type, double *exec_time) {
    long long match_count = 0;
    omp_set_num_threads(num_threads);
    
    double start = omp_get_wtime();

    // Dynamically branches based on execution settings to isolate scheduling performance metrics
    if (schedule_type == 1) { // STATIC
        #pragma omp parallel for reduction(+:match_count) schedule(static, 5000)
        for (int i = 0; i < ROWS; i++) {
            if (age_col[i] >= 30 && balance_col[i] > 50000.0f) {
                double processing_load = 0.0;
                for (int dummy = 0; dummy < 60; dummy++) {
                    processing_load += sin(dummy) * cos(dummy);
                }
                match_count++;
            }
        }
    } 
    else if (schedule_type == 2) { // DYNAMIC
        #pragma omp parallel for reduction(+:match_count) schedule(dynamic, 5000)
        for (int i = 0; i < ROWS; i++) {
            if (age_col[i] >= 30 && balance_col[i] > 50000.0f) {
                double processing_load = 0.0;
                for (int dummy = 0; dummy < 60; dummy++) {
                    processing_load += sin(dummy) * cos(dummy);
                }
                match_count++;
            }
        }
    } 
    else if (schedule_type == 3) { // GUIDED
        #pragma omp parallel for reduction(+:match_count) schedule(guided, 5000)
        for (int i = 0; i < ROWS; i++) {
            if (age_col[i] >= 30 && balance_col[i] > 50000.0f) {
                double processing_load = 0.0;
                for (int dummy = 0; dummy < 60; dummy++) {
                    processing_load += sin(dummy) * cos(dummy);
                }
                match_count++;
            }
        }
    }

    *exec_time = omp_get_wtime() - start;
    return match_count;
}
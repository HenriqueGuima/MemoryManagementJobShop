#include <stdio.h>
#include <stdlib.h>

#define MAX_JOBS 10
#define MAX_OPERATIONS 10

// STRUCTURES
typedef struct node
{
    int machine;
    int duration;
    int start;
    int jobId;
    int opId;
} operation;

typedef struct
{
    int nOperations;
    operation operations[MAX_OPERATIONS];
} job;

// READ DATA FROM FILE
void readData(char *filename, job jobs[MAX_JOBS], int *numjobs, int *nummachines)
{
    FILE *file = fopen(filename, "r");

    // CHECK IF FILE WAS OPENED
    if (file == NULL)
    {
        fprintf(stderr, "Couldn't open file %s\n", filename);
        exit(1);
    }

    // READ NUMBER OF JOBS AND MACHINES
    fscanf(file, "%d %d", numjobs, nummachines);

    // READ JOBS
    for (int i = 0; i < *numjobs; i++)
    {
        job *job = &jobs[i];
        job->nOperations = 3;

        // DEBUG PRINT
        printf("\nJob %d has %d operations and %d machines\n", i + 1, job->nOperations, *nummachines);

        for (int j = 0; j < job->nOperations; j++)
        {
            operation *operation = &job->operations[j];
            fscanf(file, "%d %d", &operation->machine, &operation->duration);
            operation->start = -1; // INITIALIZE START TIME
            operation->jobId = i;  // STORE JOB ID
            operation->opId = j;   // STORE OPERATION ID
        }
    }

    fclose(file);
}

// SCHEDULE JOBS
void sequential(job jobs[MAX_JOBS], int numjobs, int nummachines)
{
    // ALLOCATE MEMORY
    int machineTime[MAX_JOBS] = {0};
    int jobTime[MAX_JOBS] = {0};

    for (int i = 0; i < numjobs; i++)
    {
        job *job = &jobs[i];

        for (int j = 0; j < job->nOperations; j++)
        {
            operation *operation = &job->operations[j];

            int machine = operation->machine;
            int duration = operation->duration;

            // THE START TIME OF THE CURRENT OPERATION IS THE MAXIMUM OF THE END TIME OF THE LAST OPERATION ON THE SAME MACHINE
            // AND THE END TIME OF THE LAST OPERATION IN THE SAME JOB
            operation->start = (jobTime[i] > machineTime[machine]) ? jobTime[i] : machineTime[machine];

            // UPDATE THE END TIME FOR THE CURRENT JOB AND THE CURRENT MACHINE
            jobTime[i] = operation->start + duration;
            machineTime[machine] = operation->start + duration;
        }
    }

    int max = 0;

    // GET AND STORE THE MAXIMUM TIME
    for (int i = 0; i < nummachines; i++)
    {
        if (machineTime[i] > max)
        {
            max = machineTime[i];
        }
    }

    printf("------------------\n");

    // PRINT SCHEDULED OPERATIONS BY MACHINE
    for (int m = 0; m < nummachines; m++)
    {
        printf("Machine %d:\n", m);
        for (int i = 0; i < numjobs; i++)
        {
            job *job = &jobs[i];
            for (int j = 0; j < job->nOperations; j++)
            {
                operation *operation = &job->operations[j];
                if (operation->machine == m)
                {
                    printf("\tJob %d (Operation %d) -> Start Time %d, Duration %d\n",
                           operation->jobId, operation->opId, operation->start, operation->duration);
                }
            }
        }
    }

    printf("\n\nTOTAL TIME: %d\n", max);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        return 1;
    }

    job jobs[MAX_JOBS];
    int numjobs, nummachines;

    readData(argv[1], jobs, &numjobs, &nummachines);
    sequential(jobs, numjobs, nummachines);

    return 0;
}

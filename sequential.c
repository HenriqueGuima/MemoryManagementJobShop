#include <stdio.h>
#include <stdlib.h>

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
    operation *operations;
} job;

// READ DATA FROM FILE
void readData(char *filename, job **jobs, int *numjobs, int *nummachines)
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

    // MEMORY ALLOCATION
    *jobs = (job *)malloc(*numjobs * sizeof(job));

    // READ JOBS
    for (int i = 0; i < *numjobs; i++)
    {
        job *job = *jobs + i;
        job->nOperations = 3; // Each job has 3 operations

        // DEBUG PRINT
        // printf("\nJob %d has %d operations and %d machines\n", i + 1, job->nOperations, *nummachines);

        // ALLOCATE MEMORY FOR OPERATIONS OF EACH JOB
        job->operations = (operation *)malloc(job->nOperations * sizeof(operation));

        for (int j = 0; j < job->nOperations; j++)
        {
            operation *operation = job->operations + j;
            fscanf(file, "%d %d", &operation->machine, &operation->duration);
            operation->start = -1; // Initialize start time to an invalid value
            operation->jobId = i;  // Store job ID
            operation->opId = j;   // Store operation ID
        }
    }

    fclose(file);
}

// SCHEDULE JOBS
void sequential(job *jobs, int numjobs, int nummachines)
{
    // ALLOCATE MEMORY
    int *machineTime = (int *)calloc(nummachines, sizeof(int));
    int *jobTime = (int *)calloc(numjobs, sizeof(int));

    for (int i = 0; i < numjobs; i++)
    {
        job *job = jobs + i;

        for (int j = 0; j < job->nOperations; j++)
        {
            operation *operation = job->operations + j;

            int machine = operation->machine;
            int duration = operation->duration;

            // The start time of the current operation is the maximum of the end time of the last operation on the same machine
            // and the end time of the last operation in the same job
            operation->start = (jobTime[i] > machineTime[machine]) ? jobTime[i] : machineTime[machine];

            // Update the end time for the current job and the current machine
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
            job *job = jobs + i;
            for (int j = 0; j < job->nOperations; j++)
            {
                operation *operation = job->operations + j;
                if (operation->machine == m)
                {
                    printf("\tJob %d (Operation %d) -> Start Time %d, Duration %d\n",
                           operation->jobId, operation->opId, operation->start, operation->duration);
                }
            }
        }
    }

    printf("\n\nTOTAL TIME: %d\n", max);

    free(machineTime);
    free(jobTime);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        return 1;
    }

    job *jobs;
    int numjobs, nummachines;

    readData(argv[1], &jobs, &numjobs, &nummachines);
    sequential(jobs, numjobs, nummachines);

    // FREE MEMORY
    for (int i = 0; i < numjobs; i++)
    {
        free(jobs[i].operations);
    }

    free(jobs);

    return 0;
}

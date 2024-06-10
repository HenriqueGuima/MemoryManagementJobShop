#include <stdio.h>
#include <stdlib.h>

// STRUCTURES
typedef struct node
{
    int machine;
    int duration;
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
        fscanf(file, "%d", &job->nOperations);

        // DEBUG PRINT
        printf("\nJob %d has %d operations and %d machines\n", i + 1, job->nOperations, *nummachines);

        // READ THE NUMBER OF OPERATIONS PER JOB AND ALLOCATE MEMORY FOR OPERATIONS OF EACH JOB
        if (job->nOperations > 0)
        {
            job->operations = (operation *)malloc(job->nOperations * sizeof(operation));

            for (int j = 0; j < job->nOperations; j++)
            {
                operation *operation = job->operations + j;
                fscanf(file, "%d %d", &operation->machine, &operation->duration);
            }
        }
        else
        {
            job->operations = NULL;
        }
    }

    fclose(file);
}

// SCHEDULE JOBS
void sequential(job *jobs, int numjobs, int nummachines)
{

    // DEBUG PRINT
    //  printf("numjobs: %d\n", numjobs);
    //  printf("nummachines: %d\n", nummachines);

    // ALLOCATE MEMORY
    int *machines = (int *)calloc(nummachines, sizeof(int));
    int *time = (int *)calloc(nummachines, sizeof(int));

    for (int i = 0; i < numjobs; i++)
    {

        // GET JOB
        job *job = jobs + i;

        for (int j = 0; j < job->nOperations; j++)
        {

            // GET OPERATION
            operation *operation = job->operations + j;

            // GET MACHINE AND DURATION
            int machine = operation->machine - 1;
            int duration = operation->duration;

            // IF THE MACHNE IS ALREADY SCHEDULED ADD DURATION TO THE TIME, ELSE SET THE TIME AND JOB
            if (machines[machine] == i)
            {
                time[machine] += duration;
            }
            else
            {
                time[machine] = duration;
                machines[machine] = i;
            }
        }
    }

    int max = 0;

    // GET AND STORE THE MAXIMUM TIME
    for (int i = 0; i < nummachines; i++)
    {
        if (time[i] > max)
        {
            max = time[i];
        }
    }

    printf("------------------");

    // PRINT JOB, OPERATION, MACHINE AND DURATION
    for (int i = 0; i < numjobs; i++)
    {
        job *job = jobs + i;
        printf("\nJob %d:\n", i + 1);
        for (int j = 0; j < job->nOperations; j++)
        {
            operation *operation = job->operations + j;
            printf("\tOperation %d -> Machine %d, Duration %d\n", j + 1, operation->machine, operation->duration);
        }
    }

    printf("\n\nTOTAL TIME: %d\n", max);

    free(machines);
    free(time);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
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
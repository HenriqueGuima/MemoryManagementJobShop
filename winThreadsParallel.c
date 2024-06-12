#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// STRUCTURES
typedef struct node
{
    int machine;
    int duration;
    int start;
} operation;

typedef struct
{
    int nOperations;
    operation *operations;
} job;

typedef struct
{
    int nJobs;
    job* jobs;
    int nMachines;
    int idThread;
    int nThreads;
} ThreadData;

int* machineEndTime;
int* jobEndTime;
HANDLE* mutexes;

//SCHEDULE JOBS
DWORD WINAPI parallel(LPVOID arg)
{
    ThreadData* data = (ThreadData*) arg;
    int nJobs = data->nJobs;
    job* jobs = data->jobs;
    int nMachines = data->nMachines;
    int idThread = data->idThread;
    int nThreads = data->nThreads;

    for (int i = idThread; i < nJobs; i += nThreads)
    {
        job* job = jobs + i;

        for (int j = 0; j < job->nOperations; j++)
        {
            operation* operation = job->operations + j;
            int machine = operation->machine - 1;
            int duration = operation->duration;

            WaitForSingleObject(mutexes[machine], INFINITE);
            operation->start = (jobEndTime[i] > machineEndTime[machine]) ? jobEndTime[i] : machineEndTime[machine];
            jobEndTime[i] = operation->start + duration;
            machineEndTime[machine] = operation->start + duration;
            ReleaseMutex(mutexes[machine]);
        }
    }

    return 0;
}

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

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <input file> <output file> <number of threads>\n", argv[0]);
        return 1;
    }

    int nThreads = atoi(argv[3]);
    job *jobs;
    int numjobs, nummachines;

    readData(argv[1], &jobs, &numjobs, &nummachines);

    machineEndTime = (int*) malloc(nummachines * sizeof(int));
    jobEndTime = (int*) malloc(numjobs * sizeof(int));
    mutexes = (HANDLE*) malloc(nummachines * sizeof(HANDLE));

    for (int i = 0; i < nummachines; i++)
    {
        machineEndTime[i] = 0;
        mutexes[i] = CreateMutex(NULL, FALSE, NULL);
    }

    for (int i = 0; i < numjobs; i++)
    {
        jobEndTime[i] = 0;
    }

    HANDLE* threads = (HANDLE*) malloc(nThreads * sizeof(HANDLE));
    ThreadData* data = (ThreadData*) malloc(nThreads * sizeof(ThreadData));

    for (int i = 0; i < nThreads; i++)
    {
        data[i].nJobs = numjobs;
        data[i].jobs = jobs;
        data[i].nMachines = nummachines;
        data[i].idThread = i;
        data[i].nThreads = nThreads;
        threads[i] = CreateThread(NULL, 0, parallel, &data[i], 0, NULL);
    }

    WaitForMultipleObjects(nThreads, threads, TRUE, INFINITE);

    for (int i = 0; i < nThreads; i++)
    {
        CloseHandle(threads[i]);
    }

    // PRINT RESULTS
    for (int i = 0; i < numjobs; i++)
    {
        job *job = jobs + i;
        printf("\nJob %d:\n", i + 1);
        for (int j = 0; j < job->nOperations; j++)
        {
            operation *operation = job->operations + j;
            printf("\tOperation %d -> Machine %d, Duration %d, Start Time %d\n", j + 1, operation->machine, operation->duration, operation->start);
        }
    }

    // FREE MEMORY
    for (int i = 0; i < nummachines; i++)
    {
        CloseHandle(mutexes[i]);
    }

    free(jobs);
    free(machineEndTime);
    free(jobEndTime);
    free(mutexes);
    free(threads);
    free(data);

    return 0;
}

#ifndef MAIN_H
#define MAIN_H

#define SLAVES 2

typedef struct slave{
    pid_t pid;
    int in;
    int out;
    int done_tasks;
} slave;

void create_slaves(slave * slaves, int total_slaves, char * const argv[]);
void sendFiles(slave * slaves, int total_slaves, int files_per_slave, char * const argv[], int totalTasks);
void close_fds(slave slaves[], int dim);
void killSlaves(slave * slaves, int total_slaves);

#endif
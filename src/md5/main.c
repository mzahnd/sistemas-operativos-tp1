#include <stdio.h>
#include <unistd.h>
#include <utils.h>
// #include <shared_mem.h>
#include <semaphore.h>

#include "../../include/shared.h"
#include "../util/semaphore.h"
#include "../util/shared_mem.h"
#include"main.h"


#define BUFF_MEM_SIZE 1024
#define SHM_SIZE 65536
#define SLEEP_TIME 2

int main(int argc, char **argv){
        if(argc < 2){
                perror("ERROR: incorrect amount of arguments");
        }

        int tasks = argc - 1;    //cantidad de archivos
        int assigned_tasks = 0; 
        int done_tasks = 0;
        int total_slaves = min(tasks, SLAVES);
        int files_per_slave = (tasks > total_slaves * 2)? 2 : 1;

        slave slaves[total_slaves];
        
        FILE * output_file = fopen("output.txt", "w");
        if(output_file == NULL){
                perror("ERROR: could not open output file");
        }

        char * shm = open_shared_mem("shared_mem", SHM_SIZE);
        sem_t * sem = open_sem("semaphore");

        sleep(SLEEP_TIME);       //DEBE esperar 2 segundos a que aparezca un proceso vista, si lo hace le comparte el buffer de llegada

        create_slaves(slaves, total_slaves, (char **) argv);
        
        //enviar files a los esclavos
        send_files(slaves, total_slaves, files_per_slave, (char **) argv, tasks);

        close_fds(slaves, total_slaves);
        kill_slaves(slaves, total_slaves);

        close_shared_mem(shm, "shared_mem", SHM_SIZE);
        close_sem(sem);
        fclose(output_file);

        return 0;
}

//Debe iniciar los procesos esclavos
void create_slaves(slave * slaves, int total_slaves, char * const argv[]){

        for(int i = 0; i < total_slaves; i++){
                //2 pipes por cada esclavo
                int input[2];
                int output[2];

                if(pipe(input) == -1){
                        perror("ERROR: could not create input pipe");
                }

                if(pipe(output) == -1){
                        perror("ERROR: could not create output pipe");
                }

                int pid = fork();

                if(pid == -1){
                        perror("ERROR: in fork");
                }

                if(pid == 0){   //slave

                        close(input[1]);
                        close(output[0]);

                        dup2(input[0], 0);
                        dup2(output[1], 1);

                        close(input[0]);
                        close(output[1]);
                }
                else{   //master
                        slaves[i].pid = pid;
                        slaves[i].in = input[1];
                        slaves[i].out = output[0];
                        slaves[i].done_tasks = 0;

                        close(input[0]);
                        close(output[1]);

                }


        }
        
}

void send_files(slave * slaves, int total_slaves, int files_per_slave, char * const argv[], int totalTasks) {
        int tasksSent = 0;
        int tasksFinished = 0;

        for(int currentTask=0, i=1; currentTask < (files_per_slave * total_slaves); currentTask++, i++) {
                char fileSent[BUFF_MEM_SIZE] = {0};
                strcat(fileSent, argv[i]);
                strcat(fileSent, "\n\0");

                if(write(slaves[currentTask % total_slaves].out, fileSent, strlen(fileSent)) == NULL) {
                        errorHandler("Error writing in fdPath");
                }

                tasksSent++;
                slaves[currentTask % total_slaves].done_tasks++;
        }

}

void close_fds(slave slaves[], int dim) {
	for (int i = 0; i < dim; i++) {
		if (close(slaves[i].in) == -1) {
			exit_error("ERROR closing stdin fd");
		}

		if (close(slaves[i].out) == -1) {
			exit_error("ERROR close stdout fd");
		}
	}
}

void kill_slaves(slave * slaves, int total_slaves){

    for(int i = 0; i < total_slaves; i++) {

        if(close(slaves[i].in) == -1) {
            perror("Error closing read end");
        }

        if(close(slaves[i].out) == -1) {
            perror("Error closing write end");
        }

    }
}

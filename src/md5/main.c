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

int main(int argc, char **argv){
        if(argc < 2){
                perror("ERROR: incorrect amount of arguments");
        }

        int tasks = argc - 1;    //cantidad de archivos
        int assigned_tasks = 0; 
        int done_tasks = 0;
        int total_slaves = min(tasks, SLAVES);

        slave slaves[total_slaves];
        
        FILE * output_file = fopen("output.txt", "w");
        if(output_file == NULL){
                perror("ERROR: could not open output file");
        }

        char * shm = open_shared_mem("shared_mem", SHM_SIZE);
        sem_t * sem = open_sem("semaphore");

        sleep(2);       //DEBE esperar 2 segundos a que aparezca un proceso vista, si lo hace le comparte el buffer de llegada

        create_slaves(slaves, total_slaves, (char **) argv);

        //faltaria la funcion sendFiles -> enviar files a los esclavos

        killSlaves(slaves, total_slaves);

        close_shared_mem(shm, "shared_mem", SHM_SIZE);
        close_sem(sem);
        fclose(output_file);

        return 0;
}

//DEBE iniciar los procesos esclavos
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

void killSlaves(slave * slaves, int total_slaves){

    for(int i = 0; i < total_slaves; i++) {

        if(close(slaves[i].in) == -1) {
            perror("Error closing read end");
        }

        if(close(slaves[i].out) == -1) {
            perror("Error closing write end");
        }

    }
}

void sendFiles(slave * slaves, int total_slaves, char * const argv[], int totalTasks) {

}

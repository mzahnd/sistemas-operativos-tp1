// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/**
 * This file is part of sistemas-operativos-tp1
 * Licensed under MIT License.
 * Copyright (c) 2022 Flores Levalle, M.
 *                    López, P.
 *                    Sierra Pérez, C.
 *                    Zahnd, M. E.
 */

#include <stdio.h>
#include<semaphore.h>

#define BUFF_MEM_SIZE 1024
#define SHM_SIZE 65536

int main(int argc, char **argv){
        if(argc < 2){
                perror("ERROR: incorrect amount of arguments");
        }

        int tasks = argc - 1;    //cantidad de archivos
        int assigned_tasks = 0; 
        int done_tasks = 0;
        

        FILE * output_file = fopen("output.txt", "w");
        if(output_file == NULL){
                perror("ERROR: could not open output file");
        }

        //falta el slave
        char * shm = open_shared_mem("shared_mem", SHM_SIZE);
        sem_t * sem = open_sem("semaphore");

        sleep(2);       //DEBE esperar 2 segundos a que aparezca un proceso vista, si lo hace le comparte el buffer de llegada

        close_shared_mem(shm, "shared_mem", SHM_SIZE);
        close_sem(sem);
        fclose(output_file);

        return 0;
}

//DEBE iniciar los procesos esclavos
void create_slaves(int tasks){
        
}

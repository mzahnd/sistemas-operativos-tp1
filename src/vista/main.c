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
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

#include "../../include/shared.h"
#include "../util/semaphore.h"
#include "../util/shared_mem.h"

#define BUFF_MEM_SIZE 64

void print(sem_t *sem, char *shm, size_t shm_size)
{
        size_t i = 0;
        while (i < shm_size) {
                size_t jump = 0;

                if (sem_wait(sem) == -1) {
                        perror("sem_wait failed");
                        exit(EXIT_FAILURE);
                }

                jump = printf("%s", shm);
                shm += jump;
                i += jump;
        }
}

int main(int argc, char **argv)
{
        sem_t *sem = NULL;
        char *shm;
        size_t shm_size;

        if (setvbuf(stdout, NULL, _IONBF, 0) != 0) {
                perror("Could not allocate buffer");
                exit(EXIT_FAILURE);
        }

        if (argc == 1) {
                char buff[BUFF_MEM_SIZE] = { 0 };
                if (read(STDIN, buff, BUFF_MEM_SIZE) == -1) {
                        perror("Error reading data");
                        exit(EXIT_FAILURE);
                }
                shm_size = atoi(buff);
        } else if (argc == 2) {
                shm_size = atoi(argv[1]);
        } else {
                fputs("Viewer error: Invalid number of arguments.\n", stderr);
                exit(EXIT_FAILURE);
        }

        if (shm_size <= 0) {
                fputs("Viewer error: Invalid shared memory size.\n", stderr);
                exit(EXIT_FAILURE);
        }

        sem = open_sem(SEM_NAME);
        shm = open_shared_mem(SHARED_MEM_NAME, shm_size);

        print(sem, shm, shm_size);

        close_sem(sem);
        close_shared_mem(shm, SHARED_MEM_NAME, shm_size);

        return 0;
}

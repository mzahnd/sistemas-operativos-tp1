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
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>

#include <shared.h>
#include "../util/semaphore.h"
#include "../util/shared_mem.h"

#define BUFF_MEM_SIZE 64

void print(size_t shm_size)
{
        ssize_t j = 0;

        int fd_named = open(NAMED_PIPE, O_RDONLY);
        char *buff = (char *)calloc(shm_size, sizeof(char));
        if (buff == NULL) {
                perror("buff");
                exit(2);
        }

        do {
                j = 0;
                j = read(fd_named, buff, shm_size);
                printf("%s", buff);
        } while (buff[j - 2] != END_CHAR);

        free(buff);
}

int main(int argc, char **argv)
{
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

        if (shm_size == 0) {
                fputs("Viewer error: Invalid shared memory size.\n", stderr);
                exit(EXIT_FAILURE);
        }

        print(shm_size);

        return 0;
}

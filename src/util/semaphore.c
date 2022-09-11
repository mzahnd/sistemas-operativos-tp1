// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/**
 * This file is part of sistemas-operativos-tp1
 * Licensed under MIT License.
 * Copyright (c) 2022 Flores Levalle, M.
 *                    López, P.
 *                    Sierra Pérez, C.
 *                    Zahnd, M. E.
 *
 */

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

#include "semaphore.h"

#define S_RWALL S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH

sem_t *open_sem(const char *name)
{
        sem_t *sem = sem_open(name, O_CREAT | O_RDWR, S_RWALL, 0);
        if (sem == SEM_FAILED) {
                perror("Unable to open semaphore");
                return NULL;
        }

        return sem;
}

void close_sem(sem_t *sem)
{
        if (sem == NULL) {
                return;
        }

        if (sem_close(sem) == -1) {
                perror("Error while closing semaphore");
        }
}

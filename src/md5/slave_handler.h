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

#ifndef SLAVE_HANDLER_H
#define SLAVE_HANDLER_H

#define SLAVES 3

#include <stdio.h>
#include <semaphore.h>

typedef struct slave {
        pid_t pid;
        int fd_stdin;
        int fd_stdout;
        int remaining_tasks;
} slave;

struct TASK_MANAGER {
        size_t total;
        int assigned;
        size_t done;
};

struct VIEW_SHARED {
        sem_t *sem;

        char *shm;
        size_t shm_len;
        size_t shm_offset;
};

void create_slaves(slave *slaves, size_t total_slaves, char *const files[],
                   struct TASK_MANAGER *task_mgmt);
void send_files(slave *slaves, int total_slaves, char *const files[],
                struct TASK_MANAGER *task_mgmt, struct VIEW_SHARED *view_mgmt);
int kill_slaves(slave *slaves, size_t n);

#endif /* SLAVE_HANDLER_H */

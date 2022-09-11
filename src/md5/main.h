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

#ifndef MAIN_H
#define MAIN_H

#define SLAVES 2

#include <semaphore.h>

typedef struct slave {
        pid_t pid;
        int in;
        int out;
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
};

void create_slaves(slave *slaves, size_t total_slaves, char *const files[],
                   struct TASK_MANAGER *task_mgmt);
void send_files(slave *slaves, int total_slaves, int files_per_slave,
                char *const files[], struct TASK_MANAGER *task_mgmt,
                struct VIEW_SHARED *view_mgmt);
int send_files_to_slave(slave *slave, char *const files[],
                        struct TASK_MANAGER *task_mgmt);
int read_output_from_slave(FILE *output, slave *slave, char *buffer,
                           ssize_t len, struct TASK_MANAGER *task_mgmt,
                           struct VIEW_SHARED *view_mgmt);
int close_fds(slave *slaves, size_t n);
int kill_slaves(slave *slaves, size_t n);

#endif

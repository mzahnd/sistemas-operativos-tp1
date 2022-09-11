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

typedef struct slave {
        pid_t pid;
        int in;
        int out;
        int done_tasks;
} slave;

void create_slaves(slave *slaves, int total_slaves, char *const argv[]);
void sendFiles(slave *slaves, int total_slaves, int files_per_slave,
               char *const argv[], int totalTasks);
void close_fds(slave slaves[], int dim);
void killSlaves(slave *slaves, int total_slaves);

#endif

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
#include <unistd.h>

#include "../../include/shared.h"
#include "../util/semaphore.h"
#include "../util/shared_mem.h"
#include "slave_handler.h"

#define SHM_SIZE 65536
#define SLEEP_TIME 2

int main(int argc, char **argv)
{
        if (argc < 2) {
                fprintf(stderr, "Incorrect amount of arguments\n");
                exit(EXIT_FAILURE);
        }
        struct TASK_MANAGER task_mgmt = { .total = argc - 1,
                                          .assigned = 0,
                                          .done = 0 };
        size_t total_slaves = MIN(task_mgmt.total, SLAVES);
        size_t files_per_slave = (task_mgmt.total > total_slaves * 2) ? 2 : 1;

        slave slaves[total_slaves];

        struct VIEW_SHARED view_mgmt = {
                .shm_len = SHM_SIZE,
                .shm = open_shared_mem(SHARED_MEM_NAME, view_mgmt.shm_len),
                .sem = open_sem(SEM_NAME)
        };

        // Consigna: DEBE esperar 2 segundos a que aparezca un proceso vista,
        // si lo hace le comparte el buffer de llegada
        sleep(SLEEP_TIME);

        create_slaves(slaves, total_slaves, argv + 1, &task_mgmt);

        // Enviar files a los esclavos
        send_files(slaves, total_slaves, files_per_slave, argv + 1, &task_mgmt,
                   &view_mgmt);

        close_fds(slaves, total_slaves);
        kill_slaves(slaves, total_slaves);

        close_shared_mem(view_mgmt.shm, SHARED_MEM_NAME, view_mgmt.shm_len);
        close_sem(view_mgmt.sem);

        return 0;
}

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
#include <utils.h>
#include <error_handler.h>
// #include <shared_mem.h>
#include <semaphore.h>
#include <sys/select.h>
#include <string.h>

#include "../../include/shared.h"
#include "../../include/utils.h"
#include "../util/semaphore.h"
#include "../util/shared_mem.h"
#include "main.h"

#define OUTPUT_FILE "resultado.txt"

#define BUFF_MEM_SIZE 1024
#define SHM_SIZE 65536
#define SLEEP_TIME 2

#define SLAVE_PATH "./slave"
#define FILES_PER_SLAVE 2

enum fd_rw { FD_READ = 0, FD_WRITE, N_FD_RW };

int main(int argc, char **argv)
{
        if (argc < 2) {
                fprintf(stderr, "Incorrect amount of arguments\n");
                exit(EXIT_FAILURE);
        }

        struct TASK_MANAGER task_mgmt = { 0 };
        task_mgmt.total = argc - 1; //cantidad de archivos
        task_mgmt.assigned = 0;
        task_mgmt.done = 0;
        size_t total_slaves = MIN(task_mgmt.total, SLAVES);
        size_t files_per_slave = (task_mgmt.total > total_slaves * 2) ? 2 : 1;

        slave slaves[total_slaves];

        struct VIEW_SHARED view_mgmt = { 0 };
        view_mgmt.shm_len = SHM_SIZE;
        view_mgmt.shm = open_shared_mem(SHARED_MEM_NAME, view_mgmt.shm_len);
        view_mgmt.sem = open_sem(SEM_NAME);

        sleep(SLEEP_TIME); //DEBE esperar 2 segundos a que aparezca un proceso vista, si lo hace le comparte el buffer de llegada

        create_slaves(slaves, total_slaves, argv + 1, &task_mgmt);

        //enviar files a los esclavos
        send_files(slaves, total_slaves, files_per_slave, argv + 1, &task_mgmt,
                   &view_mgmt);

        close_fds(slaves, total_slaves);
        kill_slaves(slaves, total_slaves);

        close_shared_mem(view_mgmt.shm, SHARED_MEM_NAME, view_mgmt.shm_len);
        close_sem(view_mgmt.sem);

        return 0;
}

//Debe iniciar los procesos esclavos
void create_slaves(slave *slaves, size_t total_slaves, char *const files[],
                   struct TASK_MANAGER *task_mgmt)
{
        for (int i = 0; i < total_slaves; i++) {
                // 2 pipes por cada esclavo
                int input[N_FD_RW];
                int output[N_FD_RW];

                if (pipe(input) == -1) {
                        perror("ERROR: could not create input pipe");
                        exit(EXIT_FAILURE);
                }

                if (pipe(output) == -1) {
                        perror("ERROR: could not create output pipe");
                        exit(EXIT_FAILURE);
                }

                int pid = fork();

                if (pid == -1) {
                        perror("ERROR: in fork");
                        exit(EXIT_FAILURE);
                } else if (pid == 0) { //slave
                        if (close(input[FD_WRITE]) == -1) {
                                perror("Closing file descriptor failed");
                                exit(EXIT_FAILURE);
                        }
                        if (close(output[FD_READ]) == -1) {
                                perror("Closing file descriptor failed");
                                exit(EXIT_FAILURE);
                        }

                        if (dup2(input[FD_READ], STDIN) == -1) {
                                perror("File descriptor duplication failed");
                                exit(EXIT_FAILURE);
                        }
                        if (dup2(output[FD_WRITE], STDOUT) == -1) {
                                perror("File descriptor duplication failed");
                                exit(EXIT_FAILURE);
                        }

                        if (close(input[FD_READ]) == -1) {
                                perror("Closing file descriptor failed");
                                exit(EXIT_FAILURE);
                        }
                        if (close(output[FD_WRITE]) == -1) {
                                perror("Closing file descriptor failed");
                                exit(EXIT_FAILURE);
                        }

                        // Create argv for execv
                        size_t argc =
                                2 +
                                (total_slaves > SLAVES ? FILES_PER_SLAVE : 1);
                        char **argv = (char **)calloc(argc, sizeof(char *));
                        if (argv == NULL) {
                                perror("Could not allocate memory");
                                exit(EXIT_FAILURE);
                        }

                        argv[0] = strdup(SLAVE_PATH);
                        for (size_t j = 1; j < argc + 1; ++j) {
                                argv[j] = files[task_mgmt->assigned++];
                        }
                        argv[argc - 1] = NULL;

                        if (execv(SLAVE_PATH, argv) == -1) {
                                perror("execv failed in child process");
                                free(argv[0]);
                                free(argv);
                                exit(EXIT_FAILURE);
                        }

                        free(argv[0]);
                        free(argv);
                } else { //master
                        slaves[i].pid = pid;
                        slaves[i].in = input[FD_WRITE];
                        slaves[i].out = output[FD_READ];
                        slaves[i].remaining_tasks = 0;

                        if (close(input[FD_READ]) == -1) {
                                perror("Closing file descriptor failed");
                                exit(EXIT_FAILURE);
                        }
                        if (close(output[FD_WRITE]) == -1) {
                                perror("Closing file descriptor failed");
                                exit(EXIT_FAILURE);
                        }

                        task_mgmt->assigned += FILES_PER_SLAVE;
                        if (task_mgmt->assigned > task_mgmt->total) {
                                task_mgmt->assigned = task_mgmt->total;
                        }
                }
        }
}

void send_files(slave *slaves, int total_slaves, int files_per_slave,
                char *const files[], struct TASK_MANAGER *task_mgmt,
                struct VIEW_SHARED *view_mgmt)
{
        char buffer[BUFF_MEM_SIZE] = { 0 };
        FILE *output_file = fopen(OUTPUT_FILE, "w");
        if (output_file == NULL) {
                perror("ERROR: could not open output file");
                exit(EXIT_FAILURE);
        }

        while (task_mgmt->done < task_mgmt->total) {
                int max_fd = 0, ready = 0;
                fd_set read_fd_set;
                FD_ZERO(&read_fd_set);
                struct timeval tv = { 0 };

                for (int i = 0; i < total_slaves; i++) {
                        if (slaves[i].in > 0) {
                                max_fd = MAX(max_fd, slaves[i].in);
                                FD_SET(slaves[i].in, &read_fd_set);
                        }
                }

                tv.tv_sec = SLEEP_TIME;
                tv.tv_usec = 0;

                ready = select(max_fd + 1, &read_fd_set, NULL, NULL, &tv);
                if (ready == -1) {
                        perror("Error in select()");
                        fclose(output_file);
                        exit(EXIT_FAILURE);
                } else if (ready > 0) {
                        for (int i = 0; i < total_slaves && ready > 0; i++) {
                                if (FD_ISSET(slaves[i].in, &read_fd_set) != 0) {
                                        // Recibo un archivo
                                        ssize_t dim_read = read(slaves[i].in,
                                                                buffer,
                                                                BUFF_MEM_SIZE);
                                        if (dim_read == -1) {
                                                perror("Error reading from"
                                                       " file descriptor");
                                        } else if (dim_read == 0) {
                                                // El hijo termino
                                                slaves[i].in = -1;
                                        } else {
                                                if (read_output_from_slave(
                                                            output_file,
                                                            &slaves[i], buffer,
                                                            dim_read, task_mgmt,
                                                            view_mgmt) != 0) {
                                                        fclose(output_file);
                                                        exit(EXIT_FAILURE);
                                                }
                                                task_mgmt->done++;
                                        }

                                        // Envio nuevos archivos a los esclavos
                                        if (slaves[i].remaining_tasks == 0 &&
                                            task_mgmt->done <
                                                    task_mgmt->total) {
                                                if (send_files_to_slave(
                                                            &slaves[i], files,
                                                            task_mgmt) != 0) {
                                                        fclose(output_file);
                                                        exit(EXIT_FAILURE);
                                                }
                                        }
                                        ready--;
                                }
                        }
                }
        }

        fclose(output_file);
}

int send_files_to_slave(slave *slave, char *const files[],
                        struct TASK_MANAGER *task_mgmt)
{
        size_t len = 2 + strlen(files[task_mgmt->done]);
        char *file_to_slave = (char *)calloc(len, sizeof(char));

        sprintf(file_to_slave, "%s\n", files[task_mgmt->done]);
        file_to_slave[len - 1] = '\0';

        if (write(slave->out, file_to_slave, len) == -1) {
                perror("Error sending files to slave");
                free(file_to_slave);
                return 1;
        }

        slave->remaining_tasks++;
        task_mgmt->assigned++;

        free(file_to_slave);

        return 0;
}

int read_output_from_slave(FILE *output, slave *slave, char *buffer,
                           ssize_t len, struct TASK_MANAGER *task_mgmt,
                           struct VIEW_SHARED *view_mgmt)
{
        buffer[len - 1] = '\0';

        fprintf(output, "%s\n", buffer);
        slave->remaining_tasks--;

        // View
        int wrote = sprintf(view_mgmt->shm, "%s\n", buffer);
        if (wrote < 0) {
                fprintf(stderr, "An error occurred while writting "
                                "the shared memory\n");
                return 1;
        }

        view_mgmt->shm += wrote;
        sem_post(view_mgmt->sem);

        return 0;
}

void send_init_files(slave *slaves, int total_slaves, int initial_paths,
                     char *const argv[], int totalTasks, int *tasksSent)
{
        for (int currentTask = 0, i = 1; currentTask < initial_paths;
             currentTask++, i++) {
                char fileSent[BUFF_MEM_SIZE] = { 0 };
                strcat(fileSent, argv[i]);
                strcat(fileSent, "\n\0");

                if (write(slaves[currentTask % total_slaves].out, fileSent,
                          strlen(fileSent)) == -1) {
                        perror("Error writing in fdPath");
                        exit(EXIT_FAILURE);
                }

                tasksSent++;
                slaves[currentTask % total_slaves].remaining_tasks++;
        }
}

int close_fds(slave *slaves, size_t n)
{
        int ret = 0;
        for (int i = 0; i < n; i++) {
                if (close(slaves[i].in) == -1) {
                        perror("ERROR closing stdin fd");
                        ret++;
                }

                if (close(slaves[i].out) == -1) {
                        perror("ERROR close stdout fd");
                        ret++;
                }
        }

        return ret;
}

int kill_slaves(slave *slaves, size_t n)
{
        int ret = 0;
        for (int i = 0; i < n; i++) {
                if (close(slaves[i].in) == -1) {
                        perror("Error closing read end");
                        ret++;
                }

                if (close(slaves[i].out) == -1) {
                        perror("Error closing write end");
                        ret++;
                }
        }
        return ret;
}

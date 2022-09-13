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

#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

#include "../../include/shared.h"
#include "../util/semaphore.h"
#include "slave_handler.h"

#define SLAVE_PATH "./bin/slave"
#define FILES_PER_SLAVE 3

#define SLEEP_TIME 5

enum fd_rw { FD_READ = 0, FD_WRITE, N_FD_RW };

static int send_files_to_slave(slave *slave, char *const files[],
                               struct TASK_MANAGER *task_mgmt);
static int read_output_from_slave(FILE *output, slave *slave, char *buffer,
                                  ssize_t len, struct VIEW_SHARED *view_mgmt);

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

                int initial_jobs_assigned = FILES_PER_SLAVE;
                if (total_slaves < SLAVES || task_mgmt->total == SLAVES) {
                        initial_jobs_assigned = 1;
                }

                int pid = fork();

#ifdef DEBUG
                fprintf(stderr, "[Debug] Fork PID: %d\n", pid);
                sleep(1);
#endif

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

                        for (int j = 0; j < i; j++) {
                                int output_fd = slaves[j].fd_stdout;
                                int input_fd = slaves[j].fd_stdin;

                                if (close(output_fd) == -1) {
                                        perror("Closing sibling output file descriptor failed");
                                        exit(EXIT_FAILURE);
                                }
                                if (close(input_fd) == -1) {
                                        perror("Closing sibling input file descriptor failed");
                                        exit(EXIT_FAILURE);
                                }
                        }

                        // Create argv for execv
                        // 2 = SLAVE_PATH + '\0'
                        size_t argc = 2 + initial_jobs_assigned;
                        char **argv = (char **)calloc(argc, sizeof(char *));
                        if (argv == NULL) {
                                perror("Could not allocate memory");
                                exit(EXIT_FAILURE);
                        }

                        argv[0] = strdup(SLAVE_PATH);
                        for (size_t j = 1; j < argc - 1; ++j) {
                                argv[j] = files[task_mgmt->assigned++];
                        }
                        argv[argc - 1] = NULL;

#ifdef DEBUG
                        for (int j = 0; j < argc; j++) {
                                printf("SENDING ARG %d TO SLAVE [%d]: [%s]\n",
                                       j, i, argv[j]);
                        }

                        printf("Before execv slave %d\n", i);
#endif

                        if (execv(argv[0], argv) == -1) {
                                perror("execv failed in child process");
                                free(argv[0]);
                                free(argv);
                                exit(EXIT_FAILURE);
                        }

                        free(argv[0]);
                        free(argv);
                } else { //master
                        slaves[i].pid = pid;
                        slaves[i].fd_stdin = input[FD_WRITE];
                        slaves[i].fd_stdout = output[FD_READ];
                        slaves[i].remaining_tasks = initial_jobs_assigned;

                        if (close(input[FD_READ]) == -1) {
                                perror("Closing file descriptor failed");
                                exit(EXIT_FAILURE);
                        }
                        if (close(output[FD_WRITE]) == -1) {
                                perror("Closing file descriptor failed");
                                exit(EXIT_FAILURE);
                        }

                        task_mgmt->assigned += initial_jobs_assigned;
                        if (task_mgmt->assigned > task_mgmt->total) {
                                task_mgmt->assigned = task_mgmt->total;
                        }
                }
        }
}

void send_files(slave *slaves, int total_slaves, char *const files[],
                struct TASK_MANAGER *task_mgmt, struct VIEW_SHARED *view_mgmt)
{
        char buffer[BUFFER_SIZE] = { 0 };
        FILE *output_file = fopen(OUTPUT_FILE, "w");
        if (output_file == NULL) {
                perror("ERROR: could not open output file");
                exit(EXIT_FAILURE);
        }

        while (task_mgmt->done < task_mgmt->total) {
                int max_fd = 0, ready = 0;
                fd_set read_fd_set;
                FD_ZERO(&read_fd_set);
                struct timeval tv = { .tv_sec = SLEEP_TIME, .tv_usec = 0 };

                for (int i = 0; i < total_slaves; i++) {
                        if (slaves[i].fd_stdout > 0) {
                                max_fd = MAX(max_fd, slaves[i].fd_stdout);
                                FD_SET(slaves[i].fd_stdout, &read_fd_set);
                        }
                }

                ready = select(max_fd + 1, &read_fd_set, NULL, NULL, &tv);
                if (ready == -1) {
                        perror("Error in select()");
                        fclose(output_file);
                        exit(EXIT_FAILURE);
                }
                for (int i = 0; i < total_slaves; i++) {
#ifdef DEBUG
                        printf("Check Slave [%d]\n", i);
#endif
                        if (FD_ISSET(slaves[i].fd_stdout, &read_fd_set) != 0) {
#ifdef DEBUG
                                printf("Salve [%d] is ready\n", i);
#endif
                                // Recibo un archivo
                                ssize_t dim_read = read(slaves[i].fd_stdout,
                                                        buffer, BUFFER_SIZE);
                                if (dim_read == -1) {
                                        perror("Error reading from"
                                               " file descriptor");
                                } else if (dim_read > 0) {
                                        // Pensar bien que pasa si dim es 0,
                                        // Si es 0, capaz el slave murio y no hay que
                                        // pasarle nada
                                        if (read_output_from_slave(
                                                    output_file, &slaves[i],
                                                    buffer, dim_read,
                                                    view_mgmt) != 0) {
                                                fclose(output_file);
                                                exit(EXIT_FAILURE);
                                        }
                                        task_mgmt->done++;
                                }

                                // Envio nuevos archivos a los esclavos
                                if (slaves[i].remaining_tasks == 0 &&
                                    task_mgmt->assigned < task_mgmt->total) {
#ifdef DEBUG
                                        printf("Por enviar file [%s] al slave [%d]\n",
                                               files[task_mgmt->assigned], i);
#endif
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

        fclose(output_file);

        sprintf(view_mgmt->shm + view_mgmt->shm_offset, "%c", END_CHAR);
        sem_post(view_mgmt->sem);
}

static int send_files_to_slave(slave *slave, char *const files[],
                               struct TASK_MANAGER *task_mgmt)
{
        size_t len = 2 + strlen(files[task_mgmt->assigned]);
        char *file_to_slave = (char *)calloc(len, sizeof(char));
        if (file_to_slave == NULL) {
                perror("Could not allocate memory");
                exit(EXIT_FAILURE);
        }

        sprintf(file_to_slave, "%s\n", files[task_mgmt->assigned]);
        file_to_slave[len - 1] = '\0';

        if (write(slave->fd_stdin, file_to_slave, len) == -1) {
                perror("Error sending files to slave");
                free(file_to_slave);
                return 1;
        }

        slave->remaining_tasks++;
        task_mgmt->assigned++;

        free(file_to_slave);

        return 0;
}

static int read_output_from_slave(FILE *output, slave *slave, char *buffer,
                                  ssize_t len, struct VIEW_SHARED *view_mgmt)
{
        buffer[len - 1] = '\0';

        char *token = strtok(buffer, DELIMITER);
        while (token != NULL) {
#ifdef DEBUG
                printf("SLAVE %d OUTPUT: %s\n", slave->pid, token);
#endif
                // View
                int wrote = sprintf(view_mgmt->shm + view_mgmt->shm_offset,
                                    "%s", token);

                // File
                fprintf(output, "%s", token);
                if (token[strlen(token) - 1] != '\n') {
                        fputc('\n', output);
                }

                //fprintf(stderr, "SHM: [%p], str: [%s]\n", (char *)(view_mgmt->shm + view_mgmt->shm_offset), (char *)(view_mgmt->shm));
                view_mgmt->shm_offset += wrote;
                slave->remaining_tasks--;

                if (wrote < 0) {
                        fprintf(stderr, "An error occurred while writting "
                                        "the shared memory\n");
                        return 1;
                }

                token = strtok(NULL, DELIMITER);
                sem_post(view_mgmt->sem);
        }

        return 0;
}

int kill_slaves(slave *slaves, size_t n)
{
        int ret = 0;
        for (int i = 0; i < n; i++) {
                if (close(slaves[i].fd_stdout) == -1) {
                        perror("Error closing read end");
                        ret++;
                }

                if (close(slaves[i].fd_stdin) == -1) {
                        perror("Error closing write end");
                        ret++;
                }
        }
        return ret;
}

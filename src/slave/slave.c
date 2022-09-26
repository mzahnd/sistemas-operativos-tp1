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

#define _XOPEN_SOURCE 500

#include <fcntl.h>
#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <shared.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_COMMAND_OUTPUT_LENGTH 128
#define COMMAND_STR "md5sum"
#define COMMAND_PARSER "sed -r 's/[[:blank:]]+.*$/ - File Name:&/g'"

void do_task(char *file_name);
void wait_more_tasks();

int main(int argc, char const *argv[])
{
        setvbuf(stdout, NULL, _IONBF, 0);

        // for (int i = 1; i < argc; i++) {
        //         do_task((char *)argv[i]);
        // }
        printf("%s", DELIMITER);
        wait_more_tasks();

        return 0;
}

void do_task(char *file_name)
{
        FILE *output = NULL;
        char command[MAX_COMMAND_LENGTH] = { 0 };
        char result[MAX_COMMAND_OUTPUT_LENGTH * sizeof(char)] = { 0 };

        snprintf(command, MAX_COMMAND_LENGTH, "%s %s | %s", COMMAND_STR,
                 file_name, COMMAND_PARSER);

        output = popen(command, "r");
        if (output == NULL) {
                fprintf(stderr, "ERROR: could not execute md5 on file %s",
                        file_name);
                exit(EXIT_FAILURE);
        }

        int result_dim =
                fread(result, sizeof(char), MAX_COMMAND_OUTPUT_LENGTH, output);

        // Avoid array overrun
        if (result_dim == MAX_COMMAND_OUTPUT_LENGTH) {
                result[result_dim - 1] = '\0';
        } else {
                result[result_dim] = '\0';
        }

        if (pclose(output) == -1) {
                fprintf(stderr, "ERROR: could not close fileDescriptor");
                exit(EXIT_FAILURE);
        }
        printf("PID: %d - md5: %s%s", getpid(), result, DELIMITER);
}

void wait_more_tasks()
{
        char buff[MAX_COMMAND_LENGTH] = { 0 };
        size_t dim = 0;
        while ((dim = read(STDIN_FILENO, buff, MAX_COMMAND_LENGTH)) > 0) {
                // Next filename can be read from stdin or a pipe
                char *nl = strchr(buff, '\n');
                if (nl != NULL && buff[nl - buff] == '\n') {
                        buff[nl - buff] = 0;
                }

                do_task(buff);
                memset(buff, 0, dim);
        }
}

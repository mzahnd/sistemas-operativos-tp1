#define _XOPEN_SOURCE 500

#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/shared.h"

#define MAX_COMMAND_LENGTH 1024
#define MAX_COMMAND_OUTPUT_LENGTH 128
#define COMMAND_STR "md5sum"
#define COMMAND_PARSER "sed -r 's/[[:blank:]]+.*$/ - File Name:&/g'"

void do_task(char *file_name);
void wait_more_tasks();

int main(int argc, char const *argv[])
{
        for (int i = 1; i < argc; i++) {
                do_task((char *)argv[i]);
        }
        wait_more_tasks();

        return 0;
}

void do_task(char *file_name)
{
        FILE *output = NULL;
        char command[MAX_COMMAND_LENGTH] = { 0 };
        char result[MAX_COMMAND_OUTPUT_LENGTH * sizeof(char)] = { 0 };
        char buffer[BUFFER_SIZE] = { 0 };

        snprintf(command, MAX_COMMAND_LENGTH, "%s %s | %s", COMMAND_STR,
                 file_name, COMMAND_PARSER);

        output = popen(command, "r");
        if (output == NULL) {
                fprintf(stderr, "ERROR: could not execute md5 on file %s",
                        file_name);
                exit(EXIT_FAILURE);
        }

        fread(result, sizeof(char), MAX_COMMAND_OUTPUT_LENGTH, output);
        result[MAX_COMMAND_OUTPUT_LENGTH - 1] = '\0';

        snprintf(buffer, BUFFER_SIZE, "PID: %d - md5: %s", getpid(), result);
        write(STDOUT, buffer, 1 + strlen(buffer));

        int fd_2 = open("tmp_pipe", O_WRONLY);
        write(fd_2, buffer, 1 + strlen(buffer));

        if (pclose(output) == -1) {
                perror("(slave) Error closing output file descriptor");
                exit(EXIT_FAILURE);
        }
}

void wait_more_tasks()
{
        char buff[MAX_COMMAND_LENGTH] = { 0 };
        int dim = 0;
        while ((dim = read(STDIN_FILENO, buff, MAX_COMMAND_LENGTH)) > 0) {
                // Esto es porque lo hago a mano con la terminal, por lo que
                // tengo que apretar enter y agrega un \n.
                // Posiblemente haya que sacarlo
                char *nl = strchr(buff, '\n');
                if (buff[nl - buff] == '\n') {
                        buff[nl - buff] = 0;
                }
                do_task(buff);
                memset(buff, 0, dim);
        }
}

#define _XOPEN_SOURCE 500

#define MAX_COMMAND_LENGTH 1024
#define MAX_COMMAND_OUTPUT_LENGTH 64
#define COMMAND_STR "md5sum"
#define COMMAND_PARSER "sed -r 's/ .*\..*/ - File Name:&/g' | sed 's/\n/\"\"/'"

// #include <slave.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void do_task(char *file_name);
void wait_more_tasks();

int main(int argc, char const *argv[])
{
        for (int i = 1; i < argc; i++) {
                do_task((char *)argv[i]);
        }
        wait_more_tasks();
}

void do_task(char *file_name)
{
        char command[MAX_COMMAND_LENGTH];
        sprintf(command, "md5sum %s | sed -r 's/ .*\\..*/ - File Name:&/g'",
                file_name);
        FILE *output = popen(command, "r");

        if (output == NULL) {
                fprintf(stderr, "ERROR: could not execute md5 on file %s",
                        file_name);
                exit(1);
        }

        char result[MAX_COMMAND_OUTPUT_LENGTH];
        fread(result, sizeof(char), MAX_COMMAND_OUTPUT_LENGTH, output);
        printf("PID: %d - md5: %s", getpid(), result);
}

void wait_more_tasks()
{
        char buff[MAX_COMMAND_LENGTH] = { 0 };
        int dim;
        while ((dim = read(STDIN_FILENO, buff, MAX_COMMAND_LENGTH)) > 0) {
                // Esto es porque lo hago a mano con la terminal, por lo que 
                // tengo que apretar enter y agrega un \n. 
                // Posiblemente haya que sacarlo
                buff[dim - 1] = 0;
                do_task(buff);
                memset(buff, 0, dim);
        }
}

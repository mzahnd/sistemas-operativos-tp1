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
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "shared_mem.h"

#define S_RWALL S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH

char *open_shared_mem(char *name, size_t shm_len)
{
        int fd = shm_open(name, O_CREAT | O_RDWR, S_RWALL);
        if (fd == -1) {
                perror("Unable to open shared memory");
                return NULL;
        }

        if (ftruncate(fd, shm_len) == -1) {
                perror("ftruncate");
                return NULL;
        }

        void *shm =
                mmap(NULL, shm_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (shm == MAP_FAILED) {
                perror("mmap failed");
                return NULL;
        }

        if (close(fd) == -1) {
                perror("Unable to close file descriptor");
                return NULL;
        }

        return shm;
}

void close_shared_mem(char *shm, char *name, size_t shm_len)
{
        if (munmap(shm, shm_len) == -1) {
                perror("Unable to unmap shared memory");
        }

        if (shm_unlink(name) == -1) {
                if (errno != ENOENT) {
                        perror("Unable to close shared memory");
                }
        }
}

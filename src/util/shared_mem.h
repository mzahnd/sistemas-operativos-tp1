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
#ifndef SHARED_MEM_H
#define SHARED_MEM_H 1

char *open_shared_mem(char *name, size_t shm_len);
void close_shared_mem(char *shm, char *name, size_t shm_len);

#endif /* SHARED_MEM_H */

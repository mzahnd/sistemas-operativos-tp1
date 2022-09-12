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
#ifndef SHARED_H
#define SHARED_H 1

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) > (y) ? (y) : (x))

#define STDIN 0
#define STDOUT 1

#define SEM_NAME "stallman_ninjas"
#define SHARED_MEM_NAME "torvalds_nunchucks"

#define OUTPUT_FILE "resultado.txt"
#define BUFFER_SIZE 1023 // select() man specifies fds up to 1023 bytes

#define DELIMITER "§"
#define END_CHAR 0x1e

#endif /* SHARED_H */

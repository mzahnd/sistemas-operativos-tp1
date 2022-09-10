#!/bin/bash
# SPDX-License-Identifier: MIT
# 
# Copyright (c) 2022 Martín E. Zahnd
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to 
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is 
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in 
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
#
readonly SCRIPT_DIR="$(dirname ${BASH_SOURCE[0]})"
pushd $SCRIPT_DIR &> /dev/null

SOURCES=("../src" "../include")

for folder in "${SOURCES[@]}"; do
    echo -e "\e[34m\e[1mFormatting files in $folder\e[0m"
    
    find "$folder/" -type f -regex ".*\.[ch]" -exec clang-format -i {} \; 
    
    if [ $? -eq 0 ]; then
        echo -e "\e[32m\e[1mSucceded.\e[0m"
    else
        echo -e "\e[31m\e[1mFailed.\e[0m"
    fi
done

popd &> /dev/null

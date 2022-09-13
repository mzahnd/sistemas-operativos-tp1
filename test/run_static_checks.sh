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

cd ../

EXECS=("bin/md5" "bin/slave")
TEST_FILE="Makefile"

echo -e "\e[34m\e[1mCppCheck\e[0m"

cppcheck \
    --quiet \
    --enable=all \
    --force \
    --inconclusive \
    --suppress=missingIncludeSystem \
    "src/" > "test/cppcheck_out.log"

echo -en "\e[32m\e[1mDone.\e[0m"
echo " See 'cppcheck_out.log'"
echo ""

echo -e "\e[34m\e[1mPVS Studio\e[0m"
pvs-studio-analyzer trace -- make && \
pvs-studio-analyzer analyze && \
plog-converter \
    -a '64:1,2,3;GA:1,2,3;OP:1,2,3' \
    -t tasklist \
    -o "test/report.tasks" \
    PVS-Studio.log

echo -en "\e[32m\e[1mDone.\e[0m"
echo " See 'report.tasks'"
echo ""

echo -e "\e[34m\e[1mValgrind\e[0m"

for bin_file in "${EXECS[@]}"; do
    OUTPUT_LOG="test/valgrind_out_${bin_file##*/}.log"
    valgrind \
        --leak-check=yes \
        $bin_file $TEST_FILE > $OUTPUT_LOG
    
    echo -en "\e[32m\e[1mDone file $bin_file.\e[0m"
    echo " See '$OUTPUT_LOG'"
done

popd &> /dev/null


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

readonly PROJECT_DIR="$(dirname "$PWD")"

echo -e "\e[34m\e[1mWelcome to the Docker container!\e[0m"

docker run \
    --privileged \
    --interactive \
    --tty \
    --volume "$PROJECT_DIR:/itba" \
    --workdir '/itba/' \
    itba-so-tp1-grupo11:1.0


popd &> /dev/null


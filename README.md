# Sistemas Operativos: TP1

## Build

### Environment

In order to build and run this project, you'll need to use a special Docker
container.
There's a Bash script made to automate this process.

Grant execution permissions to `build_docker_image.sh` and run it.
```bash
chmod +x env/build_docker_image.sh
./env/build_docker_image.sh
```

### Source code

Grant execution permissions to `run_docker.sh` script and start the previously
builded docker image.
```bash
chmod +x env/run_docker.sh
./env/run_docker.sh
```

Once inside the container, build the project source code using make
```bash
make
```

## Running

There are three ways of running this project. 
All of them assume that you are inside the Docker container described above
and inside the project root folder.

- Hash (using md5) many files and store the results in a file 
  (called `resultado.txt`).
  ```bash
  # ./bin/md5 <files>
  ./bin/md5 LICENSE Makefile README.md
  # See the results
  cat resultado.txt
  ```
- Hash many files, store the results in a file and see the output using a piped
  viewer process
  ```bash
  # ./bin/md5 <files> | ./bin/vista
  ./bin/md5 LICENSE Makefile README.md | ./bin/vista
  # Results are still being kept
  cat resultado.txt
  ```
- Hash many files, store the results in a file and see the output using a
  viewer process running in another terminal
  ```bash
  # Terminal 1
  # buffer size is calculated as 1023 * number of files
  # ./bin/vista <buffer size>
  ./bin/vista 3069
  # ------------------
  # Terminal 2
  # ./bin/md5 <files> 
  ./bin/md5 LICENSE Makefile README.md
  # Again, results are being kept
  cat resultado.txt
  ```

> **NOTE**
>
> If you'd like to create many test files, run the following inside the 
> container:
>
> ```bash
> # Folder to keep files
> mkdir test_files
> cd test_files/
> # Create 300 files
> for n in {1..300}; do
>     dd if=/dev/urandom of=file$( printf %03d "$n" ).bin bs=1 count=$(( RANDOM + 1024 ))
> done
> # And to load them all in md5:
> cd ../ # Go back to project root 
> ./bin/md5 test_files/* | ./bin/vista
> ```

[From StackOverflow: "Create many files with random content"](https://unix.stackexchange.com/a/199865)

## License

This project and all its files are licensed under MIT License

> Copyright (c) 2022 Flores Levalle, M.,
>                    López, P.,
>                    Sierra Pérez, C.,
>                    Zahnd, M. E.,
> 

See [LICENSE](LICENSE) for details.

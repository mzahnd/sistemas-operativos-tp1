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

Then build the source code using Makefile
```bash
make
```
## License

This project and all its files are licensed under MIT License

> Copyright (c) 2022 Flores Levalle, M.
>                    López, P.
>                    Sierra Pérez, C.
>                    Zahnd, M. E.
> 

See [LICENSE](LICENSE) for details.

# Traffic lights

## How to setup for build

- create and `cd` into build directory
- run `setup.sh` from source directory

`setup.sh` saves paths to source and build directories and
creates `Makefile`. Then it runs `make all`.

## How to build (after setup)

- `make back` (default) builds back end
- `make front` copies front end files
- `make all` builds back end + copies front end files

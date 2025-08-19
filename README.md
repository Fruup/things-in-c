# Things in C

An explorative project where I learn to implement all kinds of things in pure C.

## Building

Thanks to [nob.h](https://github.com/tsoding/nob.h)!

```sh
clang -o nob nob.c
```

---

To build the runner:

- Install [bun](https://bun.com/)

```sh
bun install --cwd .bun
bun build --compile --minify --outfile bin/run .bun/index.ts
```

## Running

```sh
./nob 00_hello_world # or other root folder
```

---

Using the runner:

```sh
# select a project and run it
./bin/run

# run and watch for changes inside the selected project
./bin/run -w
```

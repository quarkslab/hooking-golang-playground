# Golang function hook using C and assembly

The repository contains the code presented in the article [“Let’s Go into the rabbit hole (part 1) — the challenges of dynamically hooking Golang programs”](). It demonstrates how one could hook a *Golang* function using a trampoline hook written in C and assembly.


## Purpose

The purpose of this project is to practically illustrate how one can hook a Golang function using C
and assembly as demonstrated in the blog post. It was created as a support for the article. The code was tested on a Fedora 37 64-bit Linux with Go version go1.20.3 and gcc version 12.3.1.

## How to build 
The project can be built using the `Makefile` present in the current directory:

```bash
$ make
```

This will produce in the [go-code](./go-code/) directory a C archive called `secret.a` and a header file called `secret.h` using the `cgo` compiler. These header file is needed by the logic in the [c-code](./c-code/) directory. Additionally, an executable called `secret` is produced using the Golang compiler. This is the `Go` file containing the function which will be hooked.
The utility will also produce in the `c-code` directory a shared library called `hooklib.so`. There is defined the C hook. 

To clean up the repository run:
```bash
$ make clean
```

## How to use
After compiling the project, you can execute the `go-code/secret` binary. The program will ask for input and will compare it to a hardcoded uppercase string. The problem is that you can't provide the correct string as all input is lowercased. To bypass this, you can inject the shared library into the process executing the 
`secret` program.
To load the `hooklib.so`, you'll need a shared library loader to load it into `secret` during runtime.
For example, it can be used using the [kubo injector](https://github.com/kubo/injector). Once the library is loaded, you should see the message **Injection completed, try your luck ;)**. Then you could enter the validation password "validateme" to validate the challenge.


## License

[Apache License 2.0](./LICENSE)


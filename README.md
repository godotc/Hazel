# Hazel
Hazel Engine 

Which just follow the @cherno's Game Engine series. 

But I've development it mainly on linux environment, and I use the `xmake` as the build tools x package manager.
And could save a lot time for the 3rd party package migration works...

You could use clangd base editor to code it， just make it generate:
```sh
xmake project -k compile_commands
```

## Run it
> install the xmake: https://xmake.io/#/guide/installation
> Install any compiler(gcc, clang, msvc)

```
git clone git@github.com:godotc/Hazel.git
cd Hazel
xmake build
xmake run sb
```

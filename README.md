# Hazel
Hazel Engine 

Which just follow the @TheCherno's Game Engine series. 

But I've development it mainly on linux environment, and I use the `xmake` as the build tools x package manager.
And could save a lot time for the 3rd party package migration works...

You could use clangd base editor to code it， just make it generate:
```sh
xmake project -k compile_commands
```

## Run it
> install the xmake: https://xmake.io/#/guide/installation
> Install any compiler(gcc, clang, msvc)

``` sh
git clone git@github.com:godotc/Hazel.git
git submodule update --init
cd Hazel
xmake build nut
xmake run nut
```
## Generate the compile_commands for `clangd` indexing 
```sh
xmake cfg
```


## Generate the debug artifacts for debugging

which will generate the LLDB and GDP profile  for debugging in vscode
```sh
xmake vscode
```


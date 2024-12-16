# Hazel
Hazel Engine 

Which just follow the @TheCherno's Game Engine series. 

But I've development it mostly on linux environment, and make it cross platform as possible. And I use the `xmake` as the build tools/package manager.
Which could save a lot time for the 3rd party package migration and other trivial works...

You could use clangd-based or lsp supported editor to read and edit it， just make the compile commands generate:
```sh
xmake project -k compile_commands
```

## Run it
> install the xmake: https://xmake.io/#/guide/installation
> Install any compiler(gcc, clang, msvc)

``` sh
git clone git@github.com:godotc/Hazel.git
cd Hazel
git submodule update --init
xmake build nut
xmake run nut

```
## Generate the compile_commands for `clangd` indexing and set the default configuration as `debug`
```sh
xmake cpcm
```

## Generate the debug artifacts for debugging

which will generate the LLDB and GDP profile  for debugging in vscode
```sh
xmake vscode
```


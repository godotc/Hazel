
.PHONY: cpcm

t:=nut
build_arg:=
arg:=

cpcm:
	xmake project -k compile_commands

cfg:
	xmake f -m debug -y
	xmake project -k compile_commands

r:
	xmake b $(t) $(build_arg)
	xmake r $(t) $(arg)
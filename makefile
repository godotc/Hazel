
.PHONY: cpcm

t:=nut
arg:=""

cpcm:
	xmake project -k compile_commands

cfg:
	xmake f -c
	xmake f -m debug
	xmake project -k compile_commands

r:
	xmake b $(t)
	xmake r $(t) $(arg)
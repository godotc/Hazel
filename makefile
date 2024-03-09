
.PHONY: cpcm
cpcm:
	xmake project -k compile_commands

cfg:
	xmake f -c
	xmake f -m debug
	xmake project -k compile_commands

r:
	xmake b nut
	xmake r nut
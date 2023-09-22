
.PHONY: cfg

cfg:
	xmake f -c
	xmake f -m debug
	xmake project -k compile_commands



sb:
	xmake build sb
	xmake run sb


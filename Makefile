export CC = emcc
export CFLAGS = -O2
export AR = emar
export LDFLAGS = -sENVIRONMENT=node \
  -sEXPORTED_FUNCTIONS=_main,_run_game,_resize_terminal,_restore_terminal \
  -sEXPORTED_RUNTIME_METHODS=cwrap --closure 1 -sASSERTIONS=0 -sSTRICT

tetrominic.js:
	$(MAKE) -C src tetrominic.mjs
	mv src/tetrominic.mjs lib/tetrominic.js
	mv src/tetrominic.wasm lib/

.PHONY: tetrominic.js

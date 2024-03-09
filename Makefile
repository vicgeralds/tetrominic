export CC = emcc
export CFLAGS = -O3
export AR = emar

tetrominic.js:
	$(MAKE) -C src tetrominic.js
	[ -d lib ] || mkdir lib
	mv src/tetrominic.js lib/tetrominic.cjs
	mv src/tetrominic.wasm lib/

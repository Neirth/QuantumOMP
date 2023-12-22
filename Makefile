# Compilers
NVC_COMPILER = nvc
GCC_COMPILER = gcc
ICX_COMPILER = icx
SEQ_COMPILER = gcc

# Common Flags
CC_FLAGS     = -O3 -std=c99 -ldl -Wall -Wextra -Wpedantic -fPIC -Isrc

# Compiler Specific Flags
NVC_FLAGS    = -mp=gpu -Minfo=mp -gpu=ccnative
GCC_FLAGS    = -march=native -ggdb -fopenmp
ICX_FLAGS    = -march=native -ggdb -ipo -xalderlake -qopt-zmm-usage=high -funroll-loops -qopenmp
SEQ_FLAGS    = -march=native -ggdb

# Targets
all: dijkstra_algorithm mandelbrot_algorithm compute_algorithms
dijkstra_algorithm: libdijkstra_intel.so libdijkstra_unix.so libdijkstra_sequential.so libdijkstra_nvidia.so
mandelbrot_algorithm: libmandelbrot_intel.so libmandelbrot_unix.so libmandelbrot_sequential.so libmandelbrot_nvidia.so

# Optional operations
clean:
	rm -f *.so compute_algorithms
	$(SEQ_COMPILER) $(CC_FLAGS) $(SEQ_FLAGS) -shared -o build/$@ $^

# Dijkstra Algorithm
libdijkstra_nvidia.so: src/algorithms/dijkstra/dijkstra.c src/utils/timeDiff.c
	[ -d build ] || mkdir build
	$(NVC_COMPILER) $(CC_FLAGS) $(NVC_FLAGS) -shared -o build/$@ $^

libdijkstra_intel.so: src/algorithms/dijkstra/dijkstra.c src/utils/timeDiff.c
	[ -d build ] || mkdir build
	$(ICX_COMPILER) $(CC_FLAGS) $(ICX_FLAGS) -shared -o build/$@ $^

libdijkstra_unix.so: src/algorithms/dijkstra/dijkstra.c src/utils/timeDiff.c
	[ -d build ] || mkdir build
	$(GCC_COMPILER) $(CC_FLAGS) $(GCC_FLAGS) -shared -o build/$@ $^

libdijkstra_sequential.so: src/algorithms/dijkstra/dijkstra.c src/utils/timeDiff.c
	[ -d build ] || mkdir build
	$(SEQ_COMPILER) $(CC_FLAGS) $(SEQ_FLAGS) -shared -o build/$@ $^

# Mandelbrot Algorithm
libmandelbrot_nvidia.so: src/algorithms/mandelbrot/mandelbrot.c src/utils/timeDiff.c
	[ -d build ] || mkdir build
	$(NVC_COMPILER) $(CC_FLAGS) $(NVC_FLAGS) -shared -o build/$@ $^

libmandelbrot_intel.so: src/algorithms/mandelbrot/mandelbrot.c src/utils/timeDiff.c
	[ -d build ] || mkdir build
	$(ICX_COMPILER) $(CC_FLAGS) $(ICX_FLAGS) -shared -o build/$@ $^

libmandelbrot_unix.so: src/algorithms/mandelbrot/mandelbrot.c src/utils/timeDiff.c
	[ -d build ] || mkdir build
	$(GCC_COMPILER) $(CC_FLAGS) $(GCC_FLAGS) -shared -o build/$@ $^

libmandelbrot_sequential.so: src/algorithms/mandelbrot/mandelbrot.c src/utils/timeDiff.c
	[ -d build ] || mkdir build
	$(SEQ_COMPILER) $(CC_FLAGS) $(SEQ_FLAGS) -shared -o build/$@ $^

# Compute Algorithms Main
compute_algorithms: src/main.c
	[ -d build ] || mkdir build
	$(SEQ_COMPILER) $(CC_FLAGS) $(SEQ_FLAGS) -o build/$@ $^

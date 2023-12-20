# Compilers
NVC_COMPILER = nvc
GCC_COMPILER = gcc
ICX_COMPILER = icx
SEQ_COMPILER = gcc

# Common Flags
CC_FLAGS     = -std=c99 -O3 -fPIC

# Compiler Specific Flags
NVC_FLAGS    = -mp=gpu -Minfo=mp -gpu=cc61
GCC_FLAGS    = -fopenmp
ICX_FLAGS    = -qopenmp
SEQ_FLAGS    =

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

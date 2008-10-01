debug:
	cd build/Debug && cmake -DCMAKE_BUILD_TYPE=Debug ../.. && make all && cd ../..
release:
	cd build/Release && cmake -DCMAKE_BUILD_TYPE=Release ../.. && make all && cd ../..
clean:
	cd build/Debug && make clean && cd ../..
	cd build/Release && make clean && cd ../..
all:release

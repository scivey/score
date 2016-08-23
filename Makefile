
create:
	mkdir -p build
	cd build && cmake ../ && make -j8

run: create
	./build/runner

clean:
	rm -rf build

.PHONY: clean run create

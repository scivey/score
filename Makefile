
cm:
	mkdir -p build
	cd build && cmake ../

create-runner: cm
	cd build && make runner -j8

create-bencher: cm
	cd build && make bencher -j8

run: create-runner
	./build/runner

bench: create-bencher
	./build/bencher

create-core-tests: cm deps
	cd build && make run_core_tests -j8

test: create-core-tests
	./build/run_core_tests

clean:
	rm -rf build

deps:
	bash scripts/deps.sh

.PHONY: clean run create-bencher create-runner deps create-core-tests test


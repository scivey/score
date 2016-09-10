
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

test-core: create-core-tests
	./build/run_core_tests

create-nlp-tests: cm deps
	cd build && make run_nlp_tests -j8

test-nlp: create-nlp-tests
	./build/run_nlp_tests

test: test-nlp test-core

clean:
	rm -rf build

deps:
	bash scripts/deps.sh

.PHONY: clean run create-bencher create-runner deps create-core-tests test



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


create-extract-tests: cm deps
	cd build && make run_extract_tests -j8

test-extract: create-extract-tests
	./build/run_extract_tests

create-curl-tests: cm deps
	cd build && make run_curl_tests -j8

test-curl: create-curl-tests
	./build/run_curl_tests

test-async: cm deps
	cd build && make run_async_tests -j8
	./build/run_async_tests

test: test-nlp test-core test-extract test-curl test-async

clean:
	rm -rf build

deps:
	bash scripts/deps.sh

.PHONY: clean run create-bencher create-runner deps create-core-tests test test-async
.PHONY: test-extract test-nlp test-core


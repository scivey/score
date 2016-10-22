
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


TEST_RUNNERS=run_core_tests \
	  run_async_tests \
	  run_curl_tests \
	  run_extract_tests \
	  run_html_tests \
	  run_nlp_tests \
	  run_memcached_tests

build-tests: cm deps
	cd build && make $(TEST_RUNNERS) -j8

test-core: build-tests
	./build/run_core_tests

test-nlp: build-tests
	./build/run_nlp_tests

test-extract: build-tests
	./build/run_extract_tests

test-curl: build-tests
	./build/run_curl_tests

test-async: build-tests
	./build/run_async_tests

test-html: build-tests
	./build/run_html_tests

test: test-nlp test-core test-extract test-curl test-async test-html

client-integration: cm deps
	cd build && make run_client_integration_tests -j8
	./build/run_client_integration_tests

clean:
	rm -rf build

deps:
	bash scripts/deps.sh

.PHONY: clean run create-bencher create-runner deps test client-integration

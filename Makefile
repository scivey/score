
cm:
	mkdir -p build
	cd build && cmake ../

create-runner: cm
	cd build && make runner -j8

run: create-runner
	./build/runner

build-tests: cm deps
	cd build && make unit_test_dummy -j8

test: build-tests
	./build/run_async_tests
	./build/run_core_tests
	./build/run_curl_tests
	./build/run_extract_tests
	./build/run_html_tests
	./build/run_memcached_tests
	./build/run_nlp_tests
	./build/run_bench_tests

build-integration: cm deps
	cd build && make integration_test_dummy -j8

integration: build-integration
	./build/run_client_integration_tests
	./build/run_misc_integration_tests

clean:
	rm -rf build

deps:
	bash scripts/deps.sh

.PHONY: clean run create-bencher create-runner deps test client-integration

bench: cm deps
	cd build && make bench_async -j8
	./build/bench_async

kgrind: cm deps
	cd build && make kgrind_threadpool -j8
	bash scripts/kgrind_threadpool.sh

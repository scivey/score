cm:
	mkdir -p build
	cd build && cmake ../

create-runner: cm
	cd build && make runner -j8

run: create-runner
	./build/runner

build-everything: cm deps
	cd build && make -j8

test: build-everything
	./scripts/run_built_unit_tests.sh

integration: build-everything
	./scripts/run_built_integration_tests.sh

clean:
	rm -rf build

deps:
	./scripts/deps.sh

.PHONY: clean run create-bencher create-runner deps test client-integration

bench: cm deps
	cd build && make bench_async -j8
	./build/bench_async

kgrind: cm deps
	cd build && make kgrind_threadpool -j8
	bash scripts/kgrind_threadpool.sh

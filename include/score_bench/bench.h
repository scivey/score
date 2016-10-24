#pragma once
#include "score_bench/BenchResult.h"
#include "score/func/Function.h"

namespace score { namespace bench {

using bench_func_t = score::func::Function<void>;

BenchResult runBenched(const std::string& name, bench_func_t&& benchFunc, size_t nIter);

}} // score::bench

#include "option_pricer/pricing.hpp"
#include <benchmark/benchmark.h>

static void Price(benchmark::State& state) {
    option_pricer::OptionParameters parameters;
    parameters.type = state.range(0) == 0 ? option_pricer::OptionType::call : option_pricer::OptionType::put;
    for (auto _ : state) {
        benchmark::DoNotOptimize(parameters);
        auto result = option_pricer::price(parameters);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(state.iterations());
}

BENCHMARK(Price)->Arg(0)->Arg(1)->ArgName("put");
BENCHMARK_MAIN();

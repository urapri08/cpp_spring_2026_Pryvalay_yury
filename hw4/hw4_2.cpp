#include <iostream>
#include <vector>
#include <future>
#include <cmath>
#include <coroutine>
#include <exception>
#include <stdexcept>
#include <chrono>

struct AggregationResult {
  double total_sum;
  double average_value;
  double maximum_value;
};

std::vector<double> stage_load(size_t elements_count) {
  std::vector<double> buffer;
  buffer.reserve(elements_count);
  for (size_t i = 0; i < elements_count; ++i) {
    buffer.push_back(static_cast<double>(i + 1));
  }
  return buffer;
}

std::vector<double> stage_transform(std::future<std::vector<double>> input_future) {
  std::vector<double> raw_data = input_future.get();
  std::vector<double> processed_data;
  processed_data.reserve(raw_data.size());

  for (double current_val : raw_data) {
    double transformed = std::sqrt(std::abs(current_val)) * std::log(1.0 + std::abs(current_val));
    processed_data.push_back(transformed);
  }
  return processed_data;
}

AggregationResult stage_reduce(std::vector<double> input_data) {
  AggregationResult res;
  res.total_sum = 0.0;
  res.maximum_value = -1.0;

  for (size_t i = 0; i < input_data.size(); ++i) {
    double val = input_data[i];
    res.total_sum += val;
    if (i == 0) {
      res.maximum_value = val;
    }
    else if (val > res.maximum_value) {
      res.maximum_value = val;
    }
  }

  if (input_data.empty()) {
    res.average_value = 0.0;
  }
  else {
    res.average_value = res.total_sum / static_cast<double>(input_data.size());
  }

  return res;
}

void execute_part_a(size_t input_size) {
  std::chrono::time_point<std::chrono::high_resolution_clock> clock_start = std::chrono::high_resolution_clock::now();

  std::future<std::vector<double>> load_fut = std::async(std::launch::async, stage_load, input_size);
  std::future<std::vector<double>> transform_fut = std::async(std::launch::async, stage_transform, std::move(load_fut));

  std::packaged_task<AggregationResult(std::vector<double>)> reduce_task(stage_reduce);
  std::future<AggregationResult> final_result_fut = reduce_task.get_future();

  std::thread reduce_thread(std::move(reduce_task), transform_fut.get());
  reduce_thread.join();

  AggregationResult final_output = final_result_fut.get();

  std::chrono::time_point<std::chrono::high_resolution_clock> clock_end = std::chrono::high_resolution_clock::now();
  double duration = std::chrono::duration<double>(clock_end - clock_start).count();

  std::cout << "[Part A - Futures Task] N=" << input_size
    << " | Sum: " << final_output.total_sum
    << " | Max: " << final_output.maximum_value
    << " | Time: " << duration << " s\n";
}

void producer_thread_func(std::promise<std::vector<double>> data_promise, size_t elements_count) {
  try {
    if (elements_count == 0) {
      throw std::invalid_argument("Size parameter cannot be zero in the producer.");
    }
    std::vector<double> result = stage_load(elements_count);
    data_promise.set_value(std::move(result));
  }
  catch (...) {
    data_promise.set_exception(std::current_exception());
  }
}

void execute_part_b() {
  std::promise<std::vector<double>> error_promise;
  std::future<std::vector<double>> error_future = error_promise.get_future();

  std::thread prod(producer_thread_func, std::move(error_promise), 0);
  prod.join();

  try {
    std::vector<double> received_data = error_future.get();
  }
  catch (const std::exception& error_object) {
    std::cout << "[Part B - Promise/Future] Exception caught successfully: " << error_object.what() << "\n";
  }
}

template<typename T>
struct SequenceGenerator {
  struct PromiseObject {
    T internal_value;

    std::suspend_always yield_value(T yielded_val) {
      internal_value = yielded_val;
      return {};
    }

    std::suspend_always initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }

    SequenceGenerator get_return_object() {
      return SequenceGenerator{ std::coroutine_handle<PromiseObject>::from_promise(*this) };
    }

    void unhandled_exception() { std::terminate(); }
    void return_void() {}
  };

  using promise_type = PromiseObject;
  std::coroutine_handle<PromiseObject> coroutine_handle;

  SequenceGenerator(std::coroutine_handle<PromiseObject> h) : coroutine_handle(h) {}
  ~SequenceGenerator() {
    if (coroutine_handle) {
      coroutine_handle.destroy();
    }
  }

  SequenceGenerator(const SequenceGenerator&) = delete;
  SequenceGenerator& operator=(const SequenceGenerator&) = delete;

  SequenceGenerator(SequenceGenerator&& other_gen) noexcept : coroutine_handle(other_gen.coroutine_handle) {
    other_gen.coroutine_handle = nullptr;
  }

  bool advance() {
    if (coroutine_handle) {
      coroutine_handle.resume();
      return !coroutine_handle.done();
    }
    return false;
  }

  T get_current() {
    return coroutine_handle.promise().internal_value;
  }
};

SequenceGenerator<double> coroutine_generate_data(size_t elements_count) {
  for (size_t i = 0; i < elements_count; ++i) {
    co_yield static_cast<double>(i + 1);
  }
}

void execute_part_c(size_t input_size) {
  std::chrono::time_point<std::chrono::high_resolution_clock> clock_start = std::chrono::high_resolution_clock::now();

  SequenceGenerator<double> lazy_source = coroutine_generate_data(input_size);

  AggregationResult res;
  res.total_sum = 0.0;
  res.maximum_value = -1.0;
  size_t processed_count = 0;

  while (lazy_source.advance()) {
    double current_val = lazy_source.get_current();

    double transformed = std::sqrt(std::abs(current_val)) * std::log(1.0 + std::abs(current_val));

    res.total_sum += transformed;
    if (processed_count == 0) {
      res.maximum_value = transformed;
    }
    else if (transformed > res.maximum_value) {
      res.maximum_value = transformed;
    }
    processed_count++;
  }

  std::chrono::time_point<std::chrono::high_resolution_clock> clock_end = std::chrono::high_resolution_clock::now();
  double duration = std::chrono::duration<double>(clock_end - clock_start).count();

  std::cout << "[Part C - Coroutine Yield] N=" << input_size
    << " | Sum: " << res.total_sum
    << " | Max: " << res.maximum_value
    << " | Time: " << duration << " s\n";
}

void print_analytical_report() {
  std::cout << "\n=== Analytical Report ===\n";
  std::cout << "1. The Futures model is highly reliable and simpler for executing massive bulk operations, but it suffers from memory bandwidth bottlenecks because it buffers the entire vector at each stage.\n";
  std::cout << "2. The Coroutine model heavily wins in memory efficiency (O(1) peak memory vs O(N) in futures) and code readability for streaming pipelines.\n";
  std::cout << "3. The primary bottleneck in the coroutine implementation is the context-switching overhead per element, though modern compilers optimize this well.\n";
}

int main() {
  std::cout << "--- BENCHMARK N = 100000 ---\n";
  execute_part_a(100000);
  execute_part_c(100000);

  std::cout << "\n--- BENCHMARK N = 1000000 ---\n";
  execute_part_a(1000000);
  execute_part_c(1000000);

  std::cout << "\n";
  execute_part_b();

  print_analytical_report();

  return 0;
}

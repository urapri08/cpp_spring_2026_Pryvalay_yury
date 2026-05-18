#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <set>
#include <chrono>
#include <atomic>
#include <sstream>
#include <iomanip>

struct LogRecord {
  long long timestamp;
  std::string user_id;
  std::string event_type;
  long long value;
};

long long global_event_count = 0;
long long global_value_sum = 0;
std::set<std::string> global_unique_users;
std::mutex global_mutex;

void process_chunk(std::vector<LogRecord>::iterator start_it, std::vector<LogRecord>::iterator end_it) {
  long long local_count = 0;
  long long local_sum = 0;
  std::set<std::string> local_users;

  for (std::vector<LogRecord>::iterator it = start_it; it != end_it; ++it) {
    local_count++;
    local_sum += it->value;
    local_users.insert(it->user_id);
  }

  std::lock_guard<std::mutex> lock(global_mutex);
  global_event_count += local_count;
  global_value_sum += local_sum;
  for (const std::string& user : local_users) {
    global_unique_users.insert(user);
  }
}

void progress_indicator(std::atomic<bool>& is_finished) {
  while (!is_finished.load()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    if (!is_finished.load()) {
      std::cout << "[Tracker] Worker threads are still processing...\n";
    }
  }
}

void generate_test_file(const std::string& filename, size_t num_lines) {
  std::ofstream outfile(filename);
  if (!outfile.is_open()) return;

  std::vector<std::string> test_users;
  test_users.push_back("Jetinovski");
  test_users.push_back("Epstein");
  test_users.push_back("Zimmerman");
  test_users.push_back("Farnsworth");

  for (size_t i = 0; i < num_lines; ++i) {
    outfile << 1714983800000 + i << " " << test_users[i % test_users.size()] << " CLICK 7\n";
  }
  outfile.close();
}

int main(int argc, char* argv[]) {
  size_t num_threads = 4;
  if (argc > 1) {
    num_threads = std::stoull(argv[1]);
  }

  std::string filename = "events.txt";
  std::ifstream infile(filename);
  if (!infile.is_open()) {
    std::cout << "File not found. Generating a mock dataset with 1,000,000 lines...\n";
    generate_test_file(filename, 1000000);
    infile.open(filename);
  }

  std::vector<LogRecord> dataset;
  std::string current_line;
  while (std::getline(infile, current_line)) {
    if (current_line.empty()) continue;
    std::istringstream stream(current_line);
    LogRecord record;
    stream >> record.timestamp >> record.user_id >> record.event_type >> record.value;
    dataset.push_back(record);
  }
  infile.close();

  std::cout << "Dataset loaded. Total records: " << dataset.size() << "\n";

  global_event_count = 0;
  global_value_sum = 0;
  global_unique_users.clear();

  std::chrono::time_point<std::chrono::high_resolution_clock> t1_start = std::chrono::high_resolution_clock::now();
  process_chunk(dataset.begin(), dataset.end());
  std::chrono::time_point<std::chrono::high_resolution_clock> t1_end = std::chrono::high_resolution_clock::now();
  double single_thread_time = std::chrono::duration<double>(t1_end - t1_start).count();

  global_event_count = 0;
  global_value_sum = 0;
  global_unique_users.clear();

  std::atomic<bool> processing_done;
  processing_done.store(false);
  std::thread tracker(progress_indicator, std::ref(processing_done));
  tracker.detach();

  std::chrono::time_point<std::chrono::high_resolution_clock> tn_start = std::chrono::high_resolution_clock::now();

  std::vector<std::thread> thread_pool;
  size_t chunk_size = dataset.size() / num_threads;
  std::vector<LogRecord>::iterator current_iterator = dataset.begin();

  for (size_t i = 0; i < num_threads; ++i) {
    std::vector<LogRecord>::iterator chunk_end = current_iterator + chunk_size;
    if (i == num_threads - 1) {
      chunk_end = dataset.end();
    }
    thread_pool.push_back(std::thread(process_chunk, current_iterator, chunk_end));
    current_iterator = chunk_end;
  }

  for (std::thread& worker : thread_pool) {
    worker.join();
  }

  processing_done.store(true);
  std::chrono::time_point<std::chrono::high_resolution_clock> tn_end = std::chrono::high_resolution_clock::now();
  double multi_thread_time = std::chrono::duration<double>(tn_end - tn_start).count();

  std::cout << "\n=== Final Output ===\n";
  std::cout << "Total events: " << global_event_count << "\n";
  std::cout << "Total value sum: " << global_value_sum << "\n";
  std::cout << "Unique users count: " << global_unique_users.size() << "\n";

  std::cout << std::fixed << std::setprecision(4);
  std::cout << "Single-thread time (T1): " << single_thread_time << " s\n";
  std::cout << "Multi-thread time (TN): " << multi_thread_time << " s\n";
  std::cout << "Speedup estimation (T1/TN): " << single_thread_time / multi_thread_time << "x\n";

  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  return 0;
}
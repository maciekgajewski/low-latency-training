// inter-thread communication without locks

#include "consume.hh"
#include "util.hh"

#include <algorithm>
#include <atomic>
#include <iostream>
#include <mutex>
#include <numeric>
#include <random>
#include <thread>

static constexpr std::size_t DATA_SIZE = 2048;
static constexpr std::size_t CYCLES = 100'000;
static constexpr std::uint64_t LAST_VERSION =
    std::numeric_limits<std::uint64_t>::max();
static constexpr double GHz = 3.392298;
static constexpr int THREADS = 1;

using namespace std::literals;

std::mutex print_mutex;

struct SharedData {
  std::atomic<std::uint64_t> version;
  std::uint64_t written_at;
  std::array<int, DATA_SIZE / sizeof(int)> data;
  std::uint64_t checksum;
};

template <std::size_t N> int generate(std::array<int, N> &array) {
  static std::random_device rd;
  std::default_random_engine rng(rd());
  std::uniform_int_distribution<int> dist(0, 1000);

  int sum = 0;
  for (int &i : array) {
    i = dist(rng);
    sum += i;
  }
  return sum;
}

void reader(SharedData &data);

int main(int, char **) {
  SharedData data;
  data.version = 0;

  pin_to_cpu(0);

  std::vector<std::thread> reader_threads;
  reader_threads.reserve(THREADS);
  for (int i = 0; i < THREADS; i++)
    reader_threads.emplace_back([&] {
      pin_to_cpu(1);
      reader(data);
    });

  std::uint64_t min_time = std::numeric_limits<std::uint64_t>::max();

  std::array<int, DATA_SIZE / sizeof(int)> in_buffer;

  // writer
  for (int i = 0; i < CYCLES; ++i) {
    int checksum = generate(in_buffer);

    auto start = rdtscf();

    {
      data.checksum = checksum;
      std::copy(in_buffer.begin(), in_buffer.end(), data.data.begin());
      data.version++;
      data.written_at = start;
    }

    auto end = rdtscf();

    min_time = std::min(min_time, end - start);
  }

  data.version = LAST_VERSION;

  {
    std::lock_guard guard(print_mutex);
    std::cout << "done writing" << std::endl;
  }

  for (std::thread &t : reader_threads)
    t.join();
  std::cout << "Records written: " << CYCLES << ", min time: " << min_time / GHz
            << "ns" << std::endl;
  std::cout << "Bytes consumed: " << get_bytes_consumed() << std::endl;
}

void reader(SharedData &data) {

  std::uint64_t prev_version = 0;

  int first_received = 0;
  int received_records = 0;
  int errors = 0;
  std::uint64_t idle_spins = 0;
  std::uint64_t min_delay = std::numeric_limits<std::uint64_t>::max();
  std::array<int, DATA_SIZE / sizeof(int)> out_buffer;

  std::uint64_t version;
  std::uint64_t written_at;
  std::uint64_t checksum;

  while (true) {
    {
      version = data.version;

      if (version == prev_version) {
        idle_spins++;
        continue;
      }

      if (version == LAST_VERSION)
        break;

      std::copy(data.data.begin(), data.data.end(), out_buffer.begin());
      written_at = data.written_at;
      checksum = data.checksum;
    }

    auto received_at = rdtscf();
    min_delay = std::min(min_delay, received_at - written_at);

    received_records++;

    auto sum = std::accumulate(out_buffer.begin(), out_buffer.end(), 0);
    if (sum != checksum) {
      errors++;
    } else {
      consume(out_buffer.data(), out_buffer.size());
    }
    if (prev_version == 0)
      first_received = version;
    prev_version = version;
  }

  {
    std::lock_guard guard(print_mutex);
    std::cout << "records received: " << received_records
              << ", first seen: " << first_received
              << ", min delay: " << min_delay / GHz << "ns , errors: " << errors
              << ", idle spins: " << idle_spins << std::endl;
  }
}

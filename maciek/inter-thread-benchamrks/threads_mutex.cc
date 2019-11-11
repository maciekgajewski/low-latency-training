// inter-thread communiation wit mutex

#include "util.hh"

#include <algorithm>
#include <iostream>
#include <mutex>
#include <numeric>
#include <random>
#include <thread>

#include <unistd.h>

static constexpr std::size_t DATA_SIZE = 2048;
static constexpr std::size_t CYCLES = 10'000;
static constexpr std::uint64_t LAST_VERSION =
    std::numeric_limits<std::uint64_t>::max();
static constexpr double GHz = 3.392298;
static constexpr int THREADS = 1;

using namespace std::literals;

class spinlock {
public:
  void lock() {
    while (flag.test_and_set()) {
      //_mm_pause();
      pthread_yield();
    }
  }

  void unlock() { flag.clear(); }

private:
  std::atomic_flag flag = ATOMIC_FLAG_INIT;
};

struct SharedData {
  std::mutex mutex;
  // spinlock mutex;
  std::uint64_t version;
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
      pin_to_cpu(1 + i % 3);
      reader(data);
    });

  std::uint64_t min_time = std::numeric_limits<std::uint64_t>::max();

  std::array<int, DATA_SIZE / sizeof(int)> in_buffer;

  // writer
  for (int i = 0; i < CYCLES; ++i) {
    int checksum = generate(in_buffer);

    auto start = rdtscf();

    {
      std::lock_guard guard(data.mutex);
      data.checksum = checksum;
      std::copy(in_buffer.begin(), in_buffer.end(), data.data.begin());
      data.version++;
      data.written_at = start;
    }

    auto end = rdtscf();

    min_time = std::min(min_time, end - start);

    pthread_yield();
    //    timespec req;
    //    req.tv_sec = 0;
    //    req.tv_nsec = 1000;
    //    nanosleep(&req, nullptr);
    usleep(1);
  }

  {
    std::lock_guard guard(data.mutex);
    data.version = LAST_VERSION;
    std::cout << "Finished writing" << std::endl;
  }

  for (std::thread &t : reader_threads)
    t.join();
  std::cout << "Records written: " << CYCLES << ", min time: " << min_time / GHz
            << "ns" << std::endl;
}

void reader(SharedData &data) {

  std::uint64_t prev_version = 0;

  int first_received = 0;
  int received_records = 0;
  int errors = 0;
  std::uint64_t idle_spins = 0;
  std::uint64_t min_delay = std::numeric_limits<std::uint64_t>::max();

  std::vector<std::uint64_t> delays;
  delays.reserve(CYCLES);

  std::array<int, DATA_SIZE / sizeof(int)> out_buffer;

  std::uint64_t version;
  std::uint64_t written_at;
  std::uint64_t checksum;

  while (true) {
    {
      std::lock_guard guard(data.mutex);
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
    delays.push_back(received_at - written_at);

    received_records++;

    auto sum = std::accumulate(out_buffer.begin(), out_buffer.end(), 0);
    if (sum != checksum)
      errors++;
    if (prev_version == 0)
      first_received = version;
    prev_version = version;
  }

  std::sort(delays.begin(), delays.end());

  {
    std::lock_guard guard(data.mutex);
    std::cout << "records received: " << received_records
              << ", first seen: " << first_received
              << ", delay: " << delays[0] / GHz << "ns/"
              << delays[received_records / 2] / GHz << "ns/"
              << delays[received_records - 1] / GHz
              << "ns , errors : " << errors << ", idle spins: " << idle_spins
              << std::endl;
  }
}

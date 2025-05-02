/*#include <iostream>
#include "lockfreequeue.hpp"
#include <atomic>
#include <thread>
#include <unordered_set>
#include <vector>

int main() {
    lockfreequeue<int> q;
    const int threads_count = 4;
    const int items_per_thread = 100;

    std::atomic<int> total_pushed{0};

    // 多线程入队
    std::vector<std::thread> producers;
    for (int i = 0; i < threads_count; ++i) {
        producers.emplace_back([&q, i, &total_pushed]() {
            for (int j = 0; j < items_per_thread; ++j) {
                int value = i * items_per_thread + j;
                q.enqueue(value);
                ++total_pushed;
            }
        });
    }

    for (auto& t : producers) t.join();

    std::cout << "Total pushed: " << total_pushed << std::endl;

    // 单线程出队并统计
    std::unordered_set<int> popped_values;
    int val;
    while (q.dequeue(&val)) {
        popped_values.insert(val);
    }

    std::cout << "Total popped: " << popped_values.size() << std::endl;

    // 检查是否缺失或重复
    bool success = true;
    for (int i = 0; i < threads_count * items_per_thread; ++i) {
        if (popped_values.find(i) == popped_values.end()) {
            std::cout << "Missing value: " << i << std::endl;
            success = false;
        }
    }

    if (success) {
        std::cout << "✅ Test passed: all values dequeued successfully." <<
std::endl; } else { std::cout << "❌ Test failed: some values were lost." <<
std::endl;
    }

    return 0;
}
*/
#include "lockfreequeue.hpp"
#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <unordered_set>
#include <vector>

int main() {
  lockfreequeue<int> q;
  const int threads_count = 4;
  const int items_per_thread = 10000;

  std::atomic<int> total_pushed{0};
  std::atomic<int> total_popped{0};
  std::mutex popped_mutex;
  std::vector<int> popped_values(threads_count * items_per_thread, 0);

  // 多线程入队
  std::vector<std::thread> producers;
  for (int i = 0; i < threads_count; ++i) {
    producers.emplace_back([&q, i, &total_pushed]() {
      for (int j = 0; j < items_per_thread; ++j) {
        int value = i * items_per_thread + j;
        q.enqueue(value);
        ++total_pushed;
      }
    });
  }

  // 多线程出队
  std::vector<std::thread> consumers;
  for (int i = 0; i < threads_count; ++i) {
    consumers.emplace_back(
        [&q, &popped_values, &total_popped, &popped_mutex]() {
          int val=-1;
          while (true) {
            if (q.dequeue(&val)) {
              {
                std::lock_guard<std::mutex> lock(popped_mutex);
                popped_values[val]++;
              }
              ++total_popped;
            } else {
              if (total_popped >= threads_count * items_per_thread)
                break;
              std::this_thread::yield(); // 避免忙等待
            }
          }
        });
  }

  for (auto &t : producers)
    t.join();
  for (auto &t : consumers)
    t.join();

  std::cout << "Total pushed: " << total_pushed << std::endl;
  std::cout << "Total popped: " << popped_values.size() << std::endl;

  // 检查是否缺失或重复
  bool success = true;
  for (int i = 0; i < threads_count * items_per_thread; ++i) {
    if (popped_values[i] != 1) {
      std::cout << "Wrong value: " << i << " x " << popped_values[i]
                << std::endl;
      success = false;
    }
  }

  if (success) {
    std::cout << "✅ Test passed: all values dequeued successfully."
              << std::endl;
  } else {
    std::cout << "❌ Test failed: some values were lost." << std::endl;
  }

  return 0;
}

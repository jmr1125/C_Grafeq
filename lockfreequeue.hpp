#include <atomic>
#include <memory>
template <typename T> class lockfreequeue {
private:
  struct Node {
    T data;
    std::atomic<Node *> next;
    Node(T val) : data(val), next(nullptr){};
  };
  std::atomic<Node *> head;
  std::atomic<Node *> tail;

public:
  lockfreequeue() {
    Node *dummy = new Node(T());
    head.store(dummy);
    tail.store(dummy);
  }
  ~lockfreequeue() {
    while (dequeue())
      ;
    delete head.load();
  }
  void enqueue(T value) {
    Node *new_node = new Node(value);
    while (true) {
      Node *last = tail.load();
      Node *next = last->next.load();
      if (last == tail.load()) {
        if (next == nullptr) {
          if (last->next.compare_exchange_weak(next, new_node)) {
            tail.compare_exchange_weak(last, new_node);
            return;
          }
        } else {
          tail.compare_exchange_weak(last, next);
        }
      }
    }
  }
  bool dequeue(T *result = nullptr) {
    while (true) {
      Node *first = head.load();
      Node *last = tail.load();
      Node *next = first->next.load();
      if (first == head.load()) {
        if (first == last) {
          if (next == nullptr) {
            return false;
          }
          tail.compare_exchange_weak(last, next);
        } else {
          if (head.compare_exchange_weak(first, next)) {
            if (result)
              *result = next->data;
            delete first;
            return true;
          }
        }
      }
    }
  }
};

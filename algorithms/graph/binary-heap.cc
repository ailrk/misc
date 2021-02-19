#include <array>
#include <functional>
#include <iostream>
#include <optional>
#define Debug

// always has the largest element on the top.
// Property:
//  1. A binary heap is a complete binary tree,
//  2. Keys has to be in total order.
//       review total order: transitivity, antisymmetry, and connexivity.
//       (connex implies reflexivity)
//  3. no duplicates.
//
//  The property is based on some total ordered keys.
//
// How do you represent a tree? Use the relative index from the parent to
// find it's children!
//
//         100
//        /   \
//       19    36     smash
//      /  \  /  \      =>   100, 19, 36, 17, 3, 25, 27
//     17  3 25   1
//    / \
//   2   7
//
// operations on binary heap
//  1. insertion O(1)                     | Swim up
//      add to bottom of the tree first.
//      If it's not correct order swap; Recurse until the order is right.
//  2. extract the maximums (delete the root.) O(logn)    | Sink down
//      replace the root with the last element in the  last level.
//      sink the root down if the order is wrong.
//      recurse until the heap property is restored.
//  3. search
//      just as normal binary tree.

// T needs to be total order.

template <typename T, size_t Size> class BinHeap {
private:
  std::array<T, Size> data;
  size_t bottom;

  inline size_t left_(size_t i) { return i * 2 + 1; };
  inline size_t right_(size_t i) { return i * 2 + 2; };

  // given node, we want to know it's parent idx, value, and
  // whether the node is left or right child of the parent.
  struct Parent {
    size_t idx;
    typename decltype(data)::reference value;
  };

  inline Parent parent_(size_t i) {
    if (i == 0)
      return {0, data.at(0)};

    size_t idx = static_cast<size_t>((i - 1) / 2);

    return {idx, data.at(idx)};
  }

  // maintain the invariant.
  bool swim_up(T &o, size_t idx);
  void sink_down(size_t idx);

public:
  BinHeap() : data(), bottom(0) {}

#ifdef Debug
  inline void print_data() {
    for (auto &v : data) {
      std::cout << v << ", ";
    }
    std::cout << "\n";
  }
#endif

  inline bool is_full() { return bottom >= data.size() - 1; };
  inline bool is_empty() { return bottom == 0; };
  inline bool in_range(size_t i) { return i < data.size(); }

  inline bool insert(T o);
  inline std::optional<T> extract();
  inline std::optional<size_t> search(const T &o);
};

template <typename T, size_t Size>
inline std::optional<size_t> BinHeap<T, Size>::search(const T &o) {

  for (size_t i = 0; i <= bottom; ++i) {
    if (data.at(i) == o) {
      return i;
    }
  }
  return {};
}

// swim up
// T &o should be the reference to new bottom.
template <typename T, size_t Size>
bool BinHeap<T, Size>::swim_up(T &o, size_t idx) {

  Parent parent = parent_(idx);

  // keep the order invariant.
  if (parent.value < o || parent.value < o) {
    std::swap(parent.value, o);
    return swim_up(parent.value, parent.idx);
  } else {
    return true;
  }
}

template <typename T, size_t Size>
void BinHeap<T, Size>::sink_down(size_t idx) {
  size_t left = left_(idx);
  size_t right = right_(idx);
  size_t largest = idx;

  if (in_range(left) && data.at(left) > data.at(largest)) {
    largest = left;
  }

  if (in_range(right) && data.at(right) > data.at(largest)) {
    largest = right;
  }

  if (largest != idx) {
    std::swap(data.at(idx), data.at(largest));
    sink_down(largest);
  }
}

template <typename T, size_t Size> inline bool BinHeap<T, Size>::insert(T o) {
  if (is_empty()) {
    data.at(bottom) = o;
    bottom++;
    return true;
  }

  if (is_full()) {
    return false;
  }

  if (search(o)) {
    return false;
  }

  data.at(bottom) = o;
  bottom++;

  return swim_up(data.at(bottom - 1), bottom - 1);
}

// sink
template <typename T, size_t Size>
inline std::optional<T> BinHeap<T, Size>::extract() {
  if (is_empty()) {
    return {};
  }

  T top = std::move(data.at(0));
  data.at(0) = std::move(data.at(bottom));
  bottom--;

  sink_down(0);

  return top; // RVO
}

int main() {
  BinHeap<int, 10> heap{};
  heap.insert(1);
  heap.insert(3);
#ifdef Debug
  heap.print_data();
#endif

  heap.insert(2);
  heap.insert(5);
#ifdef Debug
  heap.print_data();
#endif

  heap.insert(4);

#ifdef Debug
  heap.print_data();
#endif

  heap.insert(8);
  heap.insert(12);

#ifdef Debug
  heap.print_data();
#endif

  heap.extract();
  heap.extract();
  heap.extract();

#ifdef Debug
  heap.print_data();
#endif

  return 0;
}

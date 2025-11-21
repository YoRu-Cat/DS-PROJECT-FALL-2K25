#ifndef HEAP_H
#define HEAP_H

#include <iostream>
using namespace std;

// Max Heap template
template <typename T>
class MaxHeap
{
private:
  T *data;
  int capacity;
  int size;

  int parent(int i) { return (i - 1) / 2; }
  int leftChild(int i) { return 2 * i + 1; }
  int rightChild(int i) { return 2 * i + 2; }

  void heapifyUp(int index)
  {
    while (index > 0 && data[parent(index)] < data[index])
    {
      swap(data[index], data[parent(index)]);
      index = parent(index);
    }
  }

  void heapifyDown(int index)
  {
    int maxIndex = index;
    int left = leftChild(index);
    int right = rightChild(index);

    if (left < size && data[left] > data[maxIndex])
    {
      maxIndex = left;
    }

    if (right < size && data[right] > data[maxIndex])
    {
      maxIndex = right;
    }

    if (index != maxIndex)
    {
      swap(data[index], data[maxIndex]);
      heapifyDown(maxIndex);
    }
  }

  void swap(T &a, T &b)
  {
    T temp = a;
    a = b;
    b = temp;
  }

public:
  MaxHeap(int cap = 100)
  {
    capacity = cap;
    size = 0;
    data = new T[capacity];
  }

  ~MaxHeap()
  {
    delete[] data;
  }

  void insert(T element)
  {
    if (size >= capacity)
    {
      throw "Heap is full";
    }
    data[size] = element;
    heapifyUp(size);
    size++;
  }

  T extractMax()
  {
    if (size == 0)
    {
      throw "Heap is empty";
    }

    T max = data[0];
    data[0] = data[size - 1];
    size--;
    heapifyDown(0);
    return max;
  }

  T peekMax()
  {
    if (size == 0)
    {
      throw "Heap is empty";
    }
    return data[0];
  }

  bool isEmpty() { return size == 0; }
  int getSize() { return size; }

  void clear() { size = 0; }
};

// Priority Queue using Max Heap
template <typename T>
class PriorityQueue
{
private:
  struct PriorityItem
  {
    T data;
    int priority;

    PriorityItem() : priority(0) {}
    PriorityItem(T d, int p) : data(d), priority(p) {}

    bool operator>(const PriorityItem &other) const
    {
      return priority > other.priority;
    }

    bool operator<(const PriorityItem &other) const
    {
      return priority < other.priority;
    }
  };

  MaxHeap<PriorityItem> heap;

public:
  PriorityQueue(int cap = 100) : heap(cap) {}

  void enqueue(T element, int priority)
  {
    PriorityItem item(element, priority);
    heap.insert(item);
  }

  T dequeue()
  {
    PriorityItem item = heap.extractMax();
    return item.data;
  }

  bool isEmpty() { return heap.isEmpty(); }
  int getSize() { return heap.getSize(); }
};

#endif

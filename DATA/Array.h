#ifndef ARRAY_H
#define ARRAY_H

#include <iostream>
using namespace std;

// Template Array class - custom implementation
template <typename T>
class Array
{
private:
  T *data;
  int capacity;
  int size;

public:
  Array(int cap = 10)
  {
    capacity = cap;
    size = 0;
    data = new T[capacity];
  }

  ~Array()
  {
    delete[] data;
  }

  void add(T element)
  {
    if (size < capacity)
    {
      data[size++] = element;
    }
  }

  T get(int index)
  {
    if (index >= 0 && index < size)
    {
      return data[index];
    }
    throw "Index out of bounds";
  }

  void set(int index, T element)
  {
    if (index >= 0 && index < size)
    {
      data[index] = element;
    }
  }

  int getSize() { return size; }
  int getCapacity() { return capacity; }
  bool isEmpty() { return size == 0; }
  bool isFull() { return size == capacity; }

  void clear() { size = 0; }

  bool contains(T element)
  {
    for (int i = 0; i < size; i++)
    {
      if (data[i] == element)
        return true;
    }
    return false;
  }

  void print()
  {
    for (int i = 0; i < size; i++)
    {
      cout << data[i] << " ";
    }
    cout << endl;
  }
};

#endif

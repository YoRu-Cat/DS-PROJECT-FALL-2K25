#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>
using namespace std;

// Template Queue class - custom implementation
template <typename T>
class Queue
{
private:
  struct Node
  {
    T data;
    Node *next;
    Node(T d) : data(d), next(nullptr) {}
  };

  Node *front;
  Node *rear;
  int size;

public:
  Queue()
  {
    front = rear = nullptr;
    size = 0;
  }

  ~Queue()
  {
    while (!isEmpty())
    {
      dequeue();
    }
  }

  void enqueue(T element)
  {
    Node *newNode = new Node(element);
    if (isEmpty())
    {
      front = rear = newNode;
    }
    else
    {
      rear->next = newNode;
      rear = newNode;
    }
    size++;
  }

  T dequeue()
  {
    if (isEmpty())
    {
      throw "Queue is empty";
    }
    Node *temp = front;
    T data = front->data;
    front = front->next;
    if (front == nullptr)
    {
      rear = nullptr;
    }
    delete temp;
    size--;
    return data;
  }

  T peek()
  {
    if (isEmpty())
    {
      throw "Queue is empty";
    }
    return front->data;
  }

  bool isEmpty() { return front == nullptr; }
  int getSize() { return size; }

  void clear()
  {
    while (!isEmpty())
    {
      dequeue();
    }
  }
};

#endif

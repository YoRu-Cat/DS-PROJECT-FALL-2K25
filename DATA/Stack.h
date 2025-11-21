#ifndef STACK_H
#define STACK_H

#include <iostream>
using namespace std;

// Template Stack class - custom implementation
template <typename T>
class Stack
{
private:
  struct Node
  {
    T data;
    Node *next;
    Node(T d) : data(d), next(nullptr) {}
  };

  Node *top;
  int size;

public:
  Stack()
  {
    top = nullptr;
    size = 0;
  }

  ~Stack()
  {
    while (!isEmpty())
    {
      pop();
    }
  }

  void push(T element)
  {
    Node *newNode = new Node(element);
    newNode->next = top;
    top = newNode;
    size++;
  }

  T pop()
  {
    if (isEmpty())
    {
      throw "Stack is empty";
    }
    Node *temp = top;
    T data = top->data;
    top = top->next;
    delete temp;
    size--;
    return data;
  }

  T peek()
  {
    if (isEmpty())
    {
      throw "Stack is empty";
    }
    return top->data;
  }

  bool isEmpty() { return top == nullptr; }
  int getSize() { return size; }

  void clear()
  {
    while (!isEmpty())
    {
      pop();
    }
  }
};

#endif

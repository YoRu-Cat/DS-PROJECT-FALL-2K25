#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <iostream>
using namespace std;

// Singly Linked List
template <typename T>
class LinkedList
{
private:
  struct Node
  {
    T data;
    Node *next;
    Node(T d) : data(d), next(nullptr) {}
  };

  Node *head;
  int size;

public:
  LinkedList()
  {
    head = nullptr;
    size = 0;
  }

  ~LinkedList()
  {
    clear();
  }

  void insert(T element)
  {
    Node *newNode = new Node(element);
    if (head == nullptr)
    {
      head = newNode;
    }
    else
    {
      Node *temp = head;
      while (temp->next != nullptr)
      {
        temp = temp->next;
      }
      temp->next = newNode;
    }
    size++;
  }

  void insertAt(int index, T element)
  {
    if (index < 0 || index > size)
      return;

    Node *newNode = new Node(element);
    if (index == 0)
    {
      newNode->next = head;
      head = newNode;
    }
    else
    {
      Node *temp = head;
      for (int i = 0; i < index - 1; i++)
      {
        temp = temp->next;
      }
      newNode->next = temp->next;
      temp->next = newNode;
    }
    size++;
  }

  T get(int index)
  {
    if (index < 0 || index >= size)
    {
      throw "Index out of bounds";
    }
    Node *temp = head;
    for (int i = 0; i < index; i++)
    {
      temp = temp->next;
    }
    return temp->data;
  }

  bool remove(T element)
  {
    if (head == nullptr)
      return false;

    if (head->data == element)
    {
      Node *temp = head;
      head = head->next;
      delete temp;
      size--;
      return true;
    }

    Node *current = head;
    while (current->next != nullptr && current->next->data != element)
    {
      current = current->next;
    }

    if (current->next != nullptr)
    {
      Node *temp = current->next;
      current->next = current->next->next;
      delete temp;
      size--;
      return true;
    }
    return false;
  }

  T removeAt(int index)
  {
    if (index < 0 || index >= size)
    {
      throw "Index out of bounds";
    }

    T data;
    if (index == 0)
    {
      Node *temp = head;
      data = head->data;
      head = head->next;
      delete temp;
    }
    else
    {
      Node *temp = head;
      for (int i = 0; i < index - 1; i++)
      {
        temp = temp->next;
      }
      Node *toDelete = temp->next;
      data = toDelete->data;
      temp->next = toDelete->next;
      delete toDelete;
    }
    size--;
    return data;
  }

  bool isEmpty() { return head == nullptr; }
  int getSize() { return size; }

  void clear()
  {
    while (head != nullptr)
    {
      Node *temp = head;
      head = head->next;
      delete temp;
    }
    size = 0;
  }

  void print()
  {
    Node *temp = head;
    while (temp != nullptr)
    {
      cout << temp->data << " ";
      temp = temp->next;
    }
    cout << endl;
  }

  // Iterator support for range-based loops
  class Iterator
  {
  private:
    Node *current;

  public:
    Iterator(Node *node) : current(node) {}
    T &operator*() { return current->data; }
    Iterator &operator++()
    {
      current = current->next;
      return *this;
    }
    bool operator!=(const Iterator &other) { return current != other.current; }
  };

  Iterator begin() { return Iterator(head); }
  Iterator end() { return Iterator(nullptr); }
};

// Doubly Linked List
template <typename T>
class DoublyLinkedList
{
private:
  struct Node
  {
    T data;
    Node *prev;
    Node *next;
    Node(T d) : data(d), prev(nullptr), next(nullptr) {}
  };

  Node *head;
  Node *tail;
  int size;

public:
  DoublyLinkedList()
  {
    head = tail = nullptr;
    size = 0;
  }

  ~DoublyLinkedList()
  {
    clear();
  }

  void insert(T element)
  {
    Node *newNode = new Node(element);
    if (head == nullptr)
    {
      head = tail = newNode;
    }
    else
    {
      tail->next = newNode;
      newNode->prev = tail;
      tail = newNode;
    }
    size++;
  }

  T get(int index)
  {
    if (index < 0 || index >= size)
    {
      throw "Index out of bounds";
    }
    Node *temp = head;
    for (int i = 0; i < index; i++)
    {
      temp = temp->next;
    }
    return temp->data;
  }

  bool remove(T element)
  {
    Node *temp = head;
    while (temp != nullptr && temp->data != element)
    {
      temp = temp->next;
    }

    if (temp == nullptr)
      return false;

    if (temp == head)
    {
      head = head->next;
      if (head != nullptr)
        head->prev = nullptr;
      else
        tail = nullptr;
    }
    else if (temp == tail)
    {
      tail = tail->prev;
      tail->next = nullptr;
    }
    else
    {
      temp->prev->next = temp->next;
      temp->next->prev = temp->prev;
    }

    delete temp;
    size--;
    return true;
  }

  bool isEmpty() { return head == nullptr; }
  int getSize() { return size; }

  void clear()
  {
    while (head != nullptr)
    {
      Node *temp = head;
      head = head->next;
      delete temp;
    }
    tail = nullptr;
    size = 0;
  }

  T getPrevious(T current)
  {
    Node *temp = head;
    while (temp != nullptr && temp->data != current)
    {
      temp = temp->next;
    }
    if (temp != nullptr && temp->prev != nullptr)
    {
      return temp->prev->data;
    }
    throw "No previous element";
  }

  T getNext(T current)
  {
    Node *temp = head;
    while (temp != nullptr && temp->data != current)
    {
      temp = temp->next;
    }
    if (temp != nullptr && temp->next != nullptr)
    {
      return temp->next->data;
    }
    throw "No next element";
  }
};

// Circular Linked List
template <typename T>
class CircularLinkedList
{
private:
  struct Node
  {
    T data;
    Node *next;
    Node(T d) : data(d), next(nullptr) {}
  };

  Node *tail;
  int size;

public:
  CircularLinkedList()
  {
    tail = nullptr;
    size = 0;
  }

  ~CircularLinkedList()
  {
    clear();
  }

  void insert(T element)
  {
    Node *newNode = new Node(element);
    if (tail == nullptr)
    {
      tail = newNode;
      tail->next = tail;
    }
    else
    {
      newNode->next = tail->next;
      tail->next = newNode;
      tail = newNode;
    }
    size++;
  }

  bool isEmpty() { return tail == nullptr; }
  int getSize() { return size; }

  void clear()
  {
    if (tail == nullptr)
      return;

    Node *current = tail->next;
    while (current != tail)
    {
      Node *temp = current;
      current = current->next;
      delete temp;
    }
    delete tail;
    tail = nullptr;
    size = 0;
  }

  void print()
  {
    if (tail == nullptr)
      return;

    Node *temp = tail->next;
    do
    {
      cout << temp->data << " ";
      temp = temp->next;
    } while (temp != tail->next);
    cout << endl;
  }
};

#endif

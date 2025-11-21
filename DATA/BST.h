#ifndef BST_H
#define BST_H

#include <iostream>
using namespace std;

// Binary Search Tree template
template <typename K, typename V>
class BST
{
private:
  struct Node
  {
    K key;
    V value;
    Node *left;
    Node *right;

    Node(K k, V v) : key(k), value(v), left(nullptr), right(nullptr) {}
  };

  Node *root;
  int size;

  Node *insert(Node *node, K key, V value)
  {
    if (node == nullptr)
    {
      size++;
      return new Node(key, value);
    }

    if (key < node->key)
    {
      node->left = insert(node->left, key, value);
    }
    else if (key > node->key)
    {
      node->right = insert(node->right, key, value);
    }
    else
    {
      node->value = value; // Update existing
    }
    return node;
  }

  Node *findMin(Node *node)
  {
    while (node->left != nullptr)
    {
      node = node->left;
    }
    return node;
  }

  Node *remove(Node *node, K key)
  {
    if (node == nullptr)
      return nullptr;

    if (key < node->key)
    {
      node->left = remove(node->left, key);
    }
    else if (key > node->key)
    {
      node->right = remove(node->right, key);
    }
    else
    {
      if (node->left == nullptr)
      {
        Node *temp = node->right;
        delete node;
        size--;
        return temp;
      }
      else if (node->right == nullptr)
      {
        Node *temp = node->left;
        delete node;
        size--;
        return temp;
      }

      Node *temp = findMin(node->right);
      node->key = temp->key;
      node->value = temp->value;
      node->right = remove(node->right, temp->key);
    }
    return node;
  }

  Node *search(Node *node, K key)
  {
    if (node == nullptr || node->key == key)
    {
      return node;
    }

    if (key < node->key)
    {
      return search(node->left, key);
    }
    return search(node->right, key);
  }

  void inorder(Node *node, void (*callback)(K, V))
  {
    if (node != nullptr)
    {
      inorder(node->left, callback);
      callback(node->key, node->value);
      inorder(node->right, callback);
    }
  }

  void collectToArray(Node *node, K *keys, V *values, int &index, int maxSize)
  {
    if (node != nullptr && index < maxSize)
    {
      collectToArray(node->left, keys, values, index, maxSize);
      if (index < maxSize)
      {
        keys[index] = node->key;
        values[index] = node->value;
        index++;
      }
      collectToArray(node->right, keys, values, index, maxSize);
    }
  }

  void clear(Node *node)
  {
    if (node != nullptr)
    {
      clear(node->left);
      clear(node->right);
      delete node;
    }
  }

public:
  BST()
  {
    root = nullptr;
    size = 0;
  }

  ~BST()
  {
    clear(root);
  }

  void insert(K key, V value)
  {
    root = insert(root, key, value);
  }

  void remove(K key)
  {
    root = remove(root, key);
  }

  V *search(K key)
  {
    Node *result = search(root, key);
    if (result != nullptr)
    {
      return &(result->value);
    }
    return nullptr;
  }

  bool contains(K key)
  {
    return search(root, key) != nullptr;
  }

  int getSize() { return size; }
  bool isEmpty() { return root == nullptr; }

  void traverse(void (*callback)(K, V))
  {
    inorder(root, callback);
  }

  void getAllEntries(K *keys, V *values, int maxSize)
  {
    int index = 0;
    collectToArray(root, keys, values, index, maxSize);
  }

  void clear()
  {
    clear(root);
    root = nullptr;
    size = 0;
  }
};

#endif

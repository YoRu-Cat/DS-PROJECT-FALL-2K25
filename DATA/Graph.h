#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include "LinkedList.h"
using namespace std;

// Graph Node structure
struct GraphNode
{
  string userId;
  LinkedList<string> adjacentUsers;
  LinkedList<int> connectionStrengths;

  GraphNode(string id) : userId(id) {}
};

// Graph class for social connections
class Graph
{
private:
  struct GraphEntry
  {
    string userId;
    GraphNode *node;
    GraphEntry *next;

    GraphEntry(string id, GraphNode *n) : userId(id), node(n), next(nullptr) {}
  };

  GraphEntry *head;
  int size;

  GraphNode *findNode(string userId)
  {
    GraphEntry *current = head;
    while (current != nullptr)
    {
      if (current->userId == userId)
      {
        return current->node;
      }
      current = current->next;
    }
    return nullptr;
  }

public:
  Graph()
  {
    head = nullptr;
    size = 0;
  }

  ~Graph()
  {
    GraphEntry *current = head;
    while (current != nullptr)
    {
      GraphEntry *temp = current;
      current = current->next;
      delete temp->node;
      delete temp;
    }
  }

  void addUser(string userId)
  {
    if (findNode(userId) != nullptr)
      return;

    GraphNode *newNode = new GraphNode(userId);
    GraphEntry *newEntry = new GraphEntry(userId, newNode);
    newEntry->next = head;
    head = newEntry;
    size++;
  }

  void addConnection(string user1, string user2, int strength = 1)
  {
    GraphNode *node1 = findNode(user1);
    GraphNode *node2 = findNode(user2);

    if (node1 == nullptr || node2 == nullptr)
      return;

    node1->adjacentUsers.insert(user2);
    node1->connectionStrengths.insert(strength);

    node2->adjacentUsers.insert(user1);
    node2->connectionStrengths.insert(strength);
  }

  void removeConnection(string user1, string user2)
  {
    GraphNode *node1 = findNode(user1);
    GraphNode *node2 = findNode(user2);

    if (node1 == nullptr || node2 == nullptr)
      return;

    node1->adjacentUsers.remove(user2);
    node2->adjacentUsers.remove(user1);
  }

  bool areConnected(string user1, string user2)
  {
    GraphNode *node = findNode(user1);
    if (node == nullptr)
      return false;

    for (int i = 0; i < node->adjacentUsers.getSize(); i++)
    {
      if (node->adjacentUsers.get(i) == user2)
      {
        return true;
      }
    }
    return false;
  }

  int getConnectionStrength(string user1, string user2)
  {
    GraphNode *node = findNode(user1);
    if (node == nullptr)
      return 0;

    for (int i = 0; i < node->adjacentUsers.getSize(); i++)
    {
      if (node->adjacentUsers.get(i) == user2)
      {
        return node->connectionStrengths.get(i);
      }
    }
    return 0;
  }

  LinkedList<string> getMutualConnections(string user1, string user2)
  {
    LinkedList<string> mutualList;
    GraphNode *node1 = findNode(user1);
    GraphNode *node2 = findNode(user2);

    if (node1 == nullptr || node2 == nullptr)
      return mutualList;

    for (int i = 0; i < node1->adjacentUsers.getSize(); i++)
    {
      string connection = node1->adjacentUsers.get(i);
      for (int j = 0; j < node2->adjacentUsers.getSize(); j++)
      {
        if (connection == node2->adjacentUsers.get(j))
        {
          mutualList.insert(connection);
          break;
        }
      }
    }
    return mutualList;
  }

  int calculateSpamProbability(string sender, string receiver)
  {
    if (areConnected(sender, receiver))
    {
      int strength = getConnectionStrength(sender, receiver);
      return max(0, 100 - (strength * 20)); // Lower spam probability for stronger connections
    }

    LinkedList<string> mutuals = getMutualConnections(sender, receiver);
    if (mutuals.getSize() > 0)
    {
      return 50 - (mutuals.getSize() * 10); // Reduce spam probability based on mutual connections
    }

    return 80; // High spam probability for unknown senders
  }

  GraphNode *getNode(string userId)
  {
    return findNode(userId);
  }

  int getSize() { return size; }
};

#endif

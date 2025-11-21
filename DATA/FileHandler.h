#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include "User.h"
#include "Email.h"
#include "Graph.h"
#include "Array.h"
using namespace std;

class FileHandler
{
private:
  string usersFile;
  string emailsFile;
  string spamWordsFile;
  string socialGraphFile;

public:
  FileHandler()
  {
    usersFile = "users.txt";
    emailsFile = "email.txt";
    spamWordsFile = "spam_words.txt";
    socialGraphFile = "social_graph.txt";
  }

  // Load spam words into array
  void loadSpamWords(Array<string> *spamWords)
  {
    ifstream file(spamWordsFile);
    if (!file.is_open())
    {
      cout << "Creating default spam words file..." << endl;
      createDefaultSpamWords();
      file.open(spamWordsFile);
    }

    string line;
    if (getline(file, line))
    {
      stringstream ss(line);
      string word;
      while (getline(ss, word, ','))
      {
        if (!spamWords->isFull())
        {
          spamWords->add(word);
        }
      }
    }
    file.close();
  }

  void createDefaultSpamWords()
  {
    ofstream file(spamWordsFile);
    file << "Winner,Free,Urgent,Claim,Bonus,Limited,Exclusive,Gift,Guaranteed,Profit";
    file.close();
  }

  // Save user to file
  void saveUser(User *user)
  {
    ofstream file(usersFile, ios::app);
    if (file.is_open())
    {
      file << user->toString() << endl;
      file.close();
    }
  }

  // Load all users into BST
  void loadUsers(BST<string, User *> *usersBST)
  {
    ifstream file(usersFile);
    if (!file.is_open())
    {
      cout << "No users file found. Creating new one..." << endl;
      return;
    }

    string line;
    // Skip header row
    getline(file, line);

    while (getline(file, line))
    {
      if (line.empty())
        continue;

      stringstream ss(line);
      string userId, username, email, password, createdStr, loginStr;

      getline(ss, userId, ',');
      getline(ss, username, ',');
      getline(ss, email, ',');
      getline(ss, password, ',');
      getline(ss, createdStr, ',');
      getline(ss, loginStr, ',');

      User *user = new User(userId, username, email, password);
      // For now, use current time as placeholder since dates are strings
      user->setCreatedDate(time(0));
      user->setLastLogin(time(0));

      usersBST->insert(email, user);
    }
    file.close();
  }

  // Save all users from BST
  void saveAllUsers(BST<string, User *> *usersBST)
  {
    ofstream file(usersFile);
    if (file.is_open())
    {
      int maxUsers = 1000;
      string *keys = new string[maxUsers];
      User **values = new User *[maxUsers];

      usersBST->getAllEntries(keys, values, maxUsers);

      for (int i = 0; i < usersBST->getSize(); i++)
      {
        file << values[i]->toString() << endl;
      }

      delete[] keys;
      delete[] values;
      file.close();
    }
  }

  // Save email to file
  void saveEmail(Email *email)
  {
    ofstream file(emailsFile, ios::app);
    if (file.is_open())
    {
      file << email->toString() << endl;
      file.close();
    }
  }

  // Load emails for specific user
  void loadUserEmails(string userEmail, LinkedList<Email> *emailList)
  {
    ifstream file(emailsFile);
    if (!file.is_open())
    {
      return;
    }

    string line;
    // Skip header row
    getline(file, line);

    while (getline(file, line))
    {
      if (line.empty())
        continue;

      stringstream ss(line);
      string emailId, sender, receiver, subject, content, tsStr, readStr, spamStr, priorityStr, folder;

      getline(ss, emailId, ',');
      getline(ss, sender, ',');
      getline(ss, receiver, ',');
      getline(ss, subject, ',');
      getline(ss, content, ',');
      getline(ss, tsStr, ',');
      getline(ss, readStr, ',');
      getline(ss, spamStr, ',');
      getline(ss, priorityStr, ',');
      getline(ss, folder, ',');

      if (receiver == userEmail || sender == userEmail)
      {
        Email email(emailId, sender, receiver, subject, content);
        if (!tsStr.empty())
          email.setTimestamp(stol(tsStr));
        email.setIsRead(readStr == "true" || readStr == "1");
        email.setIsSpam(spamStr == "true" || spamStr == "1");
        if (!priorityStr.empty())
          email.setPriority(stoi(priorityStr));
        email.setFolder(folder);

        emailList->insert(email);
      }
    }
    file.close();
  }

  // Save all emails
  void saveAllEmails(LinkedList<Email> *emailList)
  {
    ofstream file(emailsFile);
    if (file.is_open())
    {
      for (int i = 0; i < emailList->getSize(); i++)
      {
        file << emailList->get(i).toString() << endl;
      }
      file.close();
    }
  }

  // Load social graph
  void loadSocialGraph(Graph *graph)
  {
    ifstream file(socialGraphFile);
    if (!file.is_open())
    {
      return;
    }

    string line;
    // Skip header row
    getline(file, line);

    while (getline(file, line))
    {
      if (line.empty())
        continue;

      stringstream ss(line);
      string user1, user2, strengthStr, dateStr;

      getline(ss, user1, ',');
      getline(ss, user2, ',');
      getline(ss, strengthStr, ',');
      getline(ss, dateStr, ',');

      graph->addUser(user1);
      graph->addUser(user2);
      if (!strengthStr.empty())
        graph->addConnection(user1, user2, stoi(strengthStr));
    }
    file.close();
  }

  // Save social graph
  void saveSocialGraph(Graph *graph)
  {
    // Implementation depends on graph traversal method
    // For simplicity, we'll implement basic save functionality
    ofstream file(socialGraphFile);
    if (file.is_open())
    {
      // This would need custom graph traversal implementation
      file.close();
    }
  }
};

#endif

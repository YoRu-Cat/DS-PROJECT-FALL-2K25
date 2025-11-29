#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <direct.h>
#include "User.h"
#include "Email.h"
#include "Graph.h"
#include "Array.h"
using namespace std;

class FileHandler
{
private:
  string databaseFolder;
  string usersFile;
  string spamWordsFile;
  string socialGraphFile;

  void createDirectory(const string &path)
  {
    _mkdir(path.c_str());
  }

  string getUserFolderPath(const string &userEmail)
  {
    return databaseFolder + "/" + userEmail;
  }

  string getFolderFilePath(const string &userEmail, const string &folderName)
  {
    return getUserFolderPath(userEmail) + "/" + folderName + ".txt";
  }

  string getContactsFilePath(const string &userEmail)
  {
    return getUserFolderPath(userEmail) + "/contacts.txt";
  }

  string getConnectionsFilePath(const string &userEmail)
  {
    return getUserFolderPath(userEmail) + "/connections.txt";
  }

public:
  FileHandler()
  {
    databaseFolder = "EmailDatabase";
    usersFile = databaseFolder + "/users.txt";
    spamWordsFile = databaseFolder + "/spam_words.txt";
    socialGraphFile = databaseFolder + "/social_graph.txt";

    createDirectory(databaseFolder);
  }

  void loadSpamWords(Array<string> *spamWords)
  {
    ifstream file(spamWordsFile);
    if (!file.is_open())
    {
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
        word.erase(0, word.find_first_not_of(" \t\n\r"));
        word.erase(word.find_last_not_of(" \t\n\r") + 1);

        if (!word.empty() && !spamWords->isFull())
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
    file << "Winner,Free,Urgent,Claim,Bonus,Limited,Exclusive,Gift,Guaranteed,Profit,Prize,Congratulations,Click here,Act now,Cash,Million";
    file.close();
  }

  void saveUser(User *user)
  {
    string userFolder = getUserFolderPath(user->getEmail());
    createDirectory(userFolder.c_str());

    bool userExists = false;
    ifstream checkFile(usersFile);
    string line;
    while (getline(checkFile, line))
    {
      if (line.find(user->getEmail()) != string::npos)
      {
        userExists = true;
        break;
      }
    }
    checkFile.close();

    if (!userExists)
    {
      ofstream file(usersFile, ios::app);
      if (file.is_open())
      {
        file << user->toString() << endl;
        file.close();
      }
    }
  }

  void loadUsers(BST<string, User *> *usersBST)
  {
    ifstream file(usersFile);
    if (!file.is_open())
    {
      ofstream newFile(usersFile);
      newFile << "UserId,Username,Email,Password,CreatedDate,LastLogin" << endl;
      newFile.close();
      return;
    }

    string line;
    getline(file, line); // Skip header

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
      user->setCreatedDate(time(0));
      user->setLastLogin(time(0));

      usersBST->insert(email, user);
    }
    file.close();
  }

  void saveAllUsers(BST<string, User *> *usersBST)
  {
    ofstream file(usersFile);
    if (file.is_open())
    {
      file << "UserId,Username,Email,Password,CreatedDate,LastLogin" << endl;

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

  void loadFolderEmails(const string &userEmail, const string &folderName, LinkedList<Email> *emailList)
  {
    string filePath = getFolderFilePath(userEmail, folderName);
    ifstream file(filePath);

    if (!file.is_open())
    {
      return;
    }

    string line;
    while (getline(file, line))
    {
      if (line.empty())
        continue;

      stringstream ss(line);
      string id, sender, receiver, subject, content, timestampStr, isReadStr, isSpamStr, priorityStr, folder;

      getline(ss, id, ',');
      getline(ss, sender, ',');
      getline(ss, receiver, ',');
      getline(ss, subject, ',');
      getline(ss, content, ',');
      getline(ss, timestampStr, ',');
      getline(ss, isReadStr, ',');
      getline(ss, isSpamStr, ',');
      getline(ss, priorityStr, ',');
      getline(ss, folder, ',');

      Email email(id, sender, receiver, subject, content);
      email.setTimestamp(atol(timestampStr.c_str()));
      email.setIsRead(isReadStr == "1");
      email.setIsSpam(isSpamStr == "1");
      email.setPriority(atoi(priorityStr.c_str()));
      email.setFolder(folder);

      emailList->insert(email);
    }
    file.close();
  }

  void loadUserEmails(string userEmail, LinkedList<Email> *emailList)
  {
    string folders[] = {"Inbox", "Sent", "Drafts", "Spam", "Trash", "Important"};

    for (int i = 0; i < 6; i++)
    {
      loadFolderEmails(userEmail, folders[i], emailList);
    }
  }

  void saveUserEmails(const string &userEmail,
                      LinkedList<Email> *inbox,
                      LinkedList<Email> *sent,
                      LinkedList<Email> *drafts,
                      LinkedList<Email> *spam,
                      LinkedList<Email> *trash,
                      LinkedList<Email> *important)
  {
    string userFolder = getUserFolderPath(userEmail);
    createDirectory(userFolder.c_str());

    string folderNames[] = {"Inbox", "Sent", "Drafts", "Spam", "Trash", "Important"};
    LinkedList<Email> *folderLists[] = {inbox, sent, drafts, spam, trash, important};

    for (int f = 0; f < 6; f++)
    {
      string filePath = getFolderFilePath(userEmail, folderNames[f]);
      ofstream file(filePath, ios::trunc);

      if (file.is_open())
      {
        for (int i = 0; i < folderLists[f]->getSize(); i++)
        {
          file << folderLists[f]->get(i).toString() << endl;
        }
        file.close();
      }
    }
  }

  void loadSocialGraph(Graph *graph)
  {
    ifstream file(socialGraphFile);
    if (!file.is_open())
    {
      return;
    }

    string line;
    while (getline(file, line))
    {
      if (line.empty())
        continue;

      stringstream ss(line);
      string user1, user2, strengthStr;

      getline(ss, user1, ',');
      getline(ss, user2, ',');
      getline(ss, strengthStr);

      graph->addUser(user1);
      graph->addUser(user2);
      if (!strengthStr.empty())
        graph->addConnection(user1, user2, stoi(strengthStr));
    }
    file.close();
  }

  void saveUserContacts(const string &userEmail, BST<string, Contact> *contacts)
  {
    string filePath = getContactsFilePath(userEmail);
    ofstream file(filePath);
    if (file.is_open())
    {
      file << "ContactId,Name,Email,Phone,InteractionCount" << endl;

      int maxContacts = 1000;
      string *keys = new string[maxContacts];
      Contact *values = new Contact[maxContacts];

      contacts->getAllEntries(keys, values, maxContacts);

      for (int i = 0; i < contacts->getSize(); i++)
      {
        file << values[i].getContactId() << ","
             << values[i].getName() << ","
             << values[i].getEmail() << ","
             << values[i].getPhone() << ","
             << values[i].getInteractionCount() << endl;
      }

      delete[] keys;
      delete[] values;
      file.close();
    }
  }

  void loadUserContacts(const string &userEmail, BST<string, Contact> *contacts)
  {
    string filePath = getContactsFilePath(userEmail);
    ifstream file(filePath);

    if (!file.is_open())
    {
      return;
    }

    string line;
    getline(file, line); // Skip header

    while (getline(file, line))
    {
      if (line.empty())
        continue;

      stringstream ss(line);
      string contactId, name, email, phone, interactionStr;

      getline(ss, contactId, ',');
      getline(ss, name, ',');
      getline(ss, email, ',');
      getline(ss, phone, ',');
      getline(ss, interactionStr);

      Contact contact(contactId, name, email, phone);
      contacts->insert(email, contact);
    }
    file.close();
  }

  void saveUserConnections(const string &userEmail, LinkedList<string> *adjacentUsers, LinkedList<int> *strengths)
  {
    string filePath = getConnectionsFilePath(userEmail);
    ofstream file(filePath);
    if (file.is_open())
    {
      file << "ConnectedUserEmail,ConnectionStrength" << endl;

      for (int i = 0; i < adjacentUsers->getSize(); i++)
      {
        file << adjacentUsers->get(i) << "," << strengths->get(i) << endl;
      }
      file.close();
    }
  }

  void loadUserConnections(const string &userEmail, LinkedList<string> *adjacentUsers, LinkedList<int> *strengths)
  {
    string filePath = getConnectionsFilePath(userEmail);
    ifstream file(filePath);

    if (!file.is_open())
    {
      return;
    }

    string line;
    getline(file, line); // Skip header

    while (getline(file, line))
    {
      if (line.empty())
        continue;

      stringstream ss(line);
      string connectedEmail, strengthStr;

      getline(ss, connectedEmail, ',');
      getline(ss, strengthStr);

      adjacentUsers->insert(connectedEmail);
      if (!strengthStr.empty())
        strengths->insert(stoi(strengthStr));
    }
    file.close();
  }

  void saveSocialGraph(Graph *graph)
  {
    ofstream file(socialGraphFile);
    if (file.is_open())
    {
      file.close();
    }
  }
};

#endif

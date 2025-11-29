#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
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
#ifdef _WIN32
    _mkdir(path.c_str());
#else
    mkdir(path.c_str(), 0777);
#endif
  }

  string getUserFolderPath(const string &userEmail)
  {
    return databaseFolder + "/" + userEmail;
  }

  string getFolderFilePath(const string &userEmail, const string &folderName)
  {
    return getUserFolderPath(userEmail) + "/" + folderName + ".txt";
  }

public:
  FileHandler()
  {
    databaseFolder = "EmailDatabase";
    usersFile = databaseFolder + "/users.txt";
    spamWordsFile = databaseFolder + "/spam_words.txt";
    socialGraphFile = databaseFolder + "/social_graph.txt";

    // Create main database folder
    createDirectory(databaseFolder);
  }

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
        // Trim whitespace
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
    // Create user's email folder
    string userFolder = getUserFolderPath(user->getEmail());
    createDirectory(userFolder);

    // Check if user already exists in file
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
      cout << "No users file found. Creating new one..." << endl;
      ofstream newFile(usersFile);
      newFile << "UserId,Username,Email,Password,CreatedDate,LastLogin" << endl;
      newFile.close();
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

  void saveEmailToFolder(const string &userEmail, const Email &email, const string &folderName)
  {
    string filePath = getFolderFilePath(userEmail, folderName);

    // Check if email already exists in file
    bool emailExists = false;
    ifstream checkFile(filePath);
    string line;
    while (getline(checkFile, line))
    {
      if (line.find(email.getEmailId()) != string::npos)
      {
        emailExists = true;
        break;
      }
    }
    checkFile.close();

    if (!emailExists)
    {
      ofstream file(filePath, ios::app);
      if (file.is_open())
      {
        file << email.toString() << endl;
        file.close();
      }
    }
  }

  void loadFolderEmails(const string &userEmail, const string &folderName, LinkedList<Email> *emailList)
  {
    string filePath = getFolderFilePath(userEmail, folderName);
    ifstream file(filePath);

    if (!file.is_open())
    {
      return; // Folder file doesn't exist yet, which is normal for new users
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

      emailList->add(email);
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
    // Create user folder if doesn't exist
    string userFolder = getUserFolderPath(userEmail);
    createDirectory(userFolder);

    // Clear and rewrite each folder file
    string folderNames[] = {"Inbox", "Sent", "Drafts", "Spam", "Trash", "Important"};
    LinkedList<Email> *folderLists[] = {inbox, sent, drafts, spam, trash, important};

    for (int f = 0; f < 6; f++)
    {
      string filePath = getFolderFilePath(userEmail, folderNames[f]);
      ofstream file(filePath, ios::trunc); // Overwrite file

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

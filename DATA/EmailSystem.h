#ifndef EMAILSYSTEM_H
#define EMAILSYSTEM_H

#include <iostream>
#include <sstream>
#include "User.h"
#include "Email.h"
#include "EmailFolder.h"
#include "FileHandler.h"
#include "Graph.h"
#include "Queue.h"
#include "Stack.h"
#include "Array.h"
using namespace std;

class EmailSystem
{
private:
  BST<string, User *> *users;
  Graph *socialGraph;
  Array<string> *spamWords;
  FileHandler *fileHandler;
  User *currentUser;
  Stack<string> *navigationHistory;
  Stack<Email> *deletedEmailsStack;
  Queue<Email> *scheduledEmails;

  // User folders
  EmailFolder *inbox;
  EmailFolder *sent;
  EmailFolder *drafts;
  EmailFolder *spam;
  EmailFolder *trash;
  EmailFolder *important;

  int nextEmailId;
  int nextUserId;

public:
  EmailSystem()
  {
    users = new BST<string, User *>();
    socialGraph = new Graph();
    spamWords = new Array<string>(20);
    fileHandler = new FileHandler();
    currentUser = nullptr;
    navigationHistory = new Stack<string>();
    deletedEmailsStack = new Stack<Email>();
    scheduledEmails = new Queue<Email>();

    inbox = new EmailFolder("Inbox");
    sent = new EmailFolder("Sent");
    drafts = new EmailFolder("Drafts");
    spam = new EmailFolder("Spam");
    trash = new EmailFolder("Trash");
    important = new EmailFolder("Important");

    nextEmailId = 1001;
    nextUserId = 1;

    loadData();
  }

  ~EmailSystem()
  {
    saveData();
    delete users;
    delete socialGraph;
    delete spamWords;
    delete fileHandler;
    delete navigationHistory;
    delete deletedEmailsStack;
    delete scheduledEmails;
    delete inbox;
    delete sent;
    delete drafts;
    delete spam;
    delete trash;
    delete important;
  }

  void loadData()
  {
    fileHandler->loadSpamWords(spamWords);
    fileHandler->loadUsers(users);
    fileHandler->loadSocialGraph(socialGraph);
  }

  void saveData()
  {
    fileHandler->saveAllUsers(users);
    fileHandler->saveSocialGraph(socialGraph);
    saveAllEmails();
  }

  void saveAllEmails()
  {
    // Save all emails from all folders to file
    ofstream file("email.txt");
    if (file.is_open())
    {
      file << "emailId,sender,receiver,subject,content,timestamp,isRead,isSpam,priority,folder" << endl;

      // Save from all folders
      EmailFolder *folders[] = {inbox, sent, drafts, spam, trash, important};
      for (int f = 0; f < 6; f++)
      {
        LinkedList<Email> *emails = folders[f]->getEmails();
        for (int i = 0; i < emails->getSize(); i++)
        {
          file << emails->get(i).toString() << endl;
        }
      }
      file.close();
    }
  }

  string generateUserId()
  {
    stringstream ss;
    ss << "U" << nextUserId++;
    return ss.str();
  }

  string generateEmailId()
  {
    stringstream ss;
    ss << "E" << nextEmailId++;
    return ss.str();
  }

  bool createAccount(string username, string email, string password)
  {
    if (users->contains(email))
    {
      cout << "Email already exists!" << endl;
      return false;
    }

    User *newUser = new User(generateUserId(), username, email, password);
    users->insert(email, newUser);
    socialGraph->addUser(email);
    fileHandler->saveUser(newUser);

    cout << "Account created successfully!" << endl;
    return true;
  }

  bool login(string email, string password)
  {
    User **userPtr = users->search(email);
    if (userPtr == nullptr)
    {
      cout << "User not found!" << endl;
      return false;
    }

    User *user = *userPtr;
    if (!user->validatePassword(password))
    {
      cout << "Invalid password!" << endl;
      return false;
    }

    currentUser = user;
    currentUser->setLastLogin(time(0));
    loadUserEmails();

    cout << "Login successful! Welcome, " << currentUser->getUsername() << endl;
    return true;
  }

  void logout()
  {
    if (currentUser != nullptr)
    {
      saveData();
      currentUser = nullptr;
      clearFolders();
    }
  }

  void loadUserEmails()
  {
    if (currentUser == nullptr)
      return;

    LinkedList<Email> allEmails;
    fileHandler->loadUserEmails(currentUser->getEmail(), &allEmails);

    // Prepare spam words array for checking incoming emails
    string spamArr[20];
    for (int i = 0; i < spamWords->getSize(); i++)
    {
      spamArr[i] = spamWords->get(i);
    }

    for (int i = 0; i < allEmails.getSize(); i++)
    {
      Email email = allEmails.get(i);
      string folder = email.getFolder();

      // Check for spam ONLY on incoming emails (where receiver is current user)
      // Don't check sent emails - user can send anything
      if (email.getReceiver() == currentUser->getEmail() &&
          email.getSender() != currentUser->getEmail())
      {
        if (email.containsSpamWords(spamArr, spamWords->getSize()))
        {
          email.setIsSpam(true);
          email.setFolder("Spam");
          folder = "Spam";
        }
      }

      if (folder == "Inbox")
        inbox->addEmail(email);
      else if (folder == "Sent")
        sent->addEmail(email);
      else if (folder == "Drafts")
        drafts->addEmail(email);
      else if (folder == "Spam")
        spam->addEmail(email);
      else if (folder == "Trash")
        trash->addEmail(email);
      else if (folder == "Important")
        important->addEmail(email);
    }
  }

  void clearFolders()
  {
    inbox->clearFolder();
    sent->clearFolder();
    drafts->clearFolder();
    spam->clearFolder();
    trash->clearFolder();
    important->clearFolder();
  }

  void composeEmail()
  {
    if (currentUser == nullptr)
      return;

    string to, subject, content;
    int priority;

    cout << "\n=== Compose Email ===" << endl;
    cout << "To: ";
    cin.ignore();
    getline(cin, to);
    cout << "Subject: ";
    getline(cin, subject);
    cout << "Content: ";
    getline(cin, content);
    cout << "Priority (0-5): ";

    if (!(cin >> priority))
    {
      cin.clear();
      cin.ignore(10000, '\n');
      cout << "Invalid priority! Using default 0." << endl;
      priority = 0;
    }
    else if (priority < 0 || priority > 5)
    {
      cout << "Priority out of range! Using default 0." << endl;
      priority = 0;
    }

    if (to.empty())
    {
      cout << "Error: Recipient is required!" << endl;
      return;
    }

    Email newEmail(generateEmailId(), currentUser->getEmail(), to, subject, content);
    newEmail.setPriority(priority);

    int choice;
    cout << "\n1. Send Now" << endl;
    cout << "2. Save as Draft" << endl;
    cout << "3. Schedule for Later" << endl;
    cout << "Choice: ";

    if (!(cin >> choice))
    {
      cin.clear();
      cin.ignore(10000, '\n');
      cout << "Invalid choice! Email discarded." << endl;
      return;
    }

    switch (choice)
    {
    case 1:
      newEmail.setFolder("Sent");
      sent->addEmail(newEmail);
      currentUser->addRecentContact(to);
      cout << "Email sent successfully!" << endl;
      fileHandler->saveEmail(&newEmail);
      break;
    case 2:
      newEmail.setFolder("Drafts");
      drafts->addEmail(newEmail);
      cout << "Email saved as draft!" << endl;
      break;
    case 3:
      scheduledEmails->enqueue(newEmail);
      cout << "Email scheduled!" << endl;
      break;
    default:
      cout << "Invalid choice! Email discarded." << endl;
    }
  }

  void viewFolder(string folderName)
  {
    navigationHistory->push(folderName);

    EmailFolder *folder = nullptr;
    if (folderName == "Inbox")
      folder = inbox;
    else if (folderName == "Sent")
      folder = sent;
    else if (folderName == "Drafts")
      folder = drafts;
    else if (folderName == "Spam")
      folder = spam;
    else if (folderName == "Trash")
      folder = trash;
    else if (folderName == "Important")
      folder = important;

    if (folder != nullptr)
    {
      folder->displayAllEmails();
    }
  }

  void viewInboxByPriority()
  {
    inbox->displayEmailsByPriority();
  }

  void searchEmail()
  {
    if (currentUser == nullptr)
      return;

    string query;
    cout << "Enter search query (sender/subject): ";
    cin.ignore();
    getline(cin, query);

    cout << "\n=== Search Results ===" << endl;
    bool found = false;

    // Search in all folders
    EmailFolder *folders[] = {inbox, sent, drafts, spam, trash, important};
    string folderNames[] = {"Inbox", "Sent", "Drafts", "Spam", "Trash", "Important"};

    for (int f = 0; f < 6; f++)
    {
      LinkedList<Email> *emails = folders[f]->getEmails();
      for (int i = 0; i < emails->getSize(); i++)
      {
        Email email = emails->get(i);
        if (email.getSender().find(query) != string::npos ||
            email.getSubject().find(query) != string::npos)
        {
          cout << "\nFolder: " << folderNames[f] << endl;
          email.display();
          found = true;
        }
      }
    }

    if (!found)
    {
      cout << "No emails found matching the query." << endl;
    }
  }

  void deleteEmail(string emailId, string folderName)
  {
    EmailFolder *folder = nullptr;
    if (folderName == "Inbox")
      folder = inbox;
    else if (folderName == "Sent")
      folder = sent;
    else if (folderName == "Drafts")
      folder = drafts;
    else if (folderName == "Spam")
      folder = spam;
    else if (folderName == "Important")
      folder = important;

    if (folder != nullptr)
    {
      try
      {
        Email email = folder->removeEmail(emailId);
        email.setFolder("Trash");
        trash->addEmail(email);
        deletedEmailsStack->push(email);
        cout << "Email moved to trash." << endl;
      }
      catch (const char *msg)
      {
        cout << msg << endl;
      }
    }
  }

  void recoverLastDeleted()
  {
    if (deletedEmailsStack->isEmpty())
    {
      cout << "No deleted emails to recover." << endl;
      return;
    }

    Email email = deletedEmailsStack->pop();
    inbox->addEmail(email);
    cout << "Email recovered successfully!" << endl;
  }

  void emptyTrash()
  {
    trash->clearFolder();
    cout << "Trash emptied successfully!" << endl;
  }

  void addContact()
  {
    if (currentUser == nullptr)
      return;

    string name, email, phone;
    cout << "\nEnter contact name: ";
    cin.ignore();
    getline(cin, name);
    cout << "Enter contact email: ";
    getline(cin, email);
    cout << "Enter contact phone (optional): ";
    getline(cin, phone);

    if (name.empty())
    {
      cout << "Error: Contact name is required!" << endl;
      return;
    }
    if (email.empty())
    {
      cout << "Error: Contact email is required!" << endl;
      return;
    }

    Contact newContact("C" + to_string(time(0)), name, email, phone);
    currentUser->addContact(newContact);
    cout << "Contact added successfully!" << endl;
  }

  void viewContacts()
  {
    if (currentUser == nullptr)
      return;

    currentUser->displayAllContacts();
  }

  void viewRecentContacts()
  {
    if (currentUser == nullptr)
      return;

    currentUser->displayRecentContacts();
  }

  void addSocialConnection(string userEmail)
  {
    if (currentUser == nullptr)
      return;

    if (users->contains(userEmail))
    {
      socialGraph->addConnection(currentUser->getEmail(), userEmail, 1);
      cout << "Connection added successfully!" << endl;
    }
    else
    {
      cout << "User not found!" << endl;
    }
  }

  void viewMutualConnections(string userEmail)
  {
    if (currentUser == nullptr)
      return;

    LinkedList<string> mutuals = socialGraph->getMutualConnections(currentUser->getEmail(), userEmail);

    cout << "\n=== Mutual Connections ===" << endl;
    if (mutuals.isEmpty())
    {
      cout << "No mutual connections found." << endl;
    }
    else
    {
      for (int i = 0; i < mutuals.getSize(); i++)
      {
        cout << (i + 1) << ". " << mutuals.get(i) << endl;
      }
    }
  }

  void displaySystemStats()
  {
    cout << "\n=== System Statistics ===" << endl;
    cout << "Total Users: " << users->getSize() << endl;
    cout << "Inbox: " << inbox->getEmailCount() << " (" << inbox->getUnreadCount() << " unread)" << endl;
    cout << "Sent: " << sent->getEmailCount() << endl;
    cout << "Drafts: " << drafts->getEmailCount() << endl;
    cout << "Spam: " << spam->getEmailCount() << endl;
    cout << "Trash: " << trash->getEmailCount() << endl;
    cout << "Important: " << important->getEmailCount() << endl;
    cout << "Scheduled: " << scheduledEmails->getSize() << endl;
    cout << "========================" << endl;
  }

  bool isLoggedIn() { return currentUser != nullptr; }
  User *getCurrentUser() { return currentUser; }

  // Folder getters for UI
  EmailFolder *getInbox() { return inbox; }
  EmailFolder *getSent() { return sent; }
  EmailFolder *getDrafts() { return drafts; }
  EmailFolder *getSpam() { return spam; }
  EmailFolder *getTrash() { return trash; }
  EmailFolder *getImportant() { return important; }
};

#endif

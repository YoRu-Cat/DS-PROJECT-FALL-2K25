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
#include "Heap.h"
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
  Stack<Email> *undoStack; // Undo functionality
  Stack<Email> *redoStack; // Redo functionality
  Queue<Email> *scheduledEmails;
  Queue<Email> *incomingEmailQueue;         // Processing queue for incoming emails
  PriorityQueue<Email> *priorityEmailQueue; // High-importance emails
  MaxHeap<Email> *timestampHeap;            // Max-heap for organizing by timestamp
  Array<string> *systemConfig;              // System configuration settings
  LinkedList<string> *activityLog;          // Recent activity log (circular)
  int activityLogMaxSize;

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
    undoStack = new Stack<Email>();
    redoStack = new Stack<Email>();
    scheduledEmails = new Queue<Email>();
    incomingEmailQueue = new Queue<Email>();
    priorityEmailQueue = new PriorityQueue<Email>(100);
    timestampHeap = new MaxHeap<Email>(100);
    systemConfig = new Array<string>(10);
    activityLog = new LinkedList<string>();
    activityLogMaxSize = 20; // Keep last 20 activities

    inbox = new EmailFolder("Inbox");
    sent = new EmailFolder("Sent");
    drafts = new EmailFolder("Drafts");
    spam = new EmailFolder("Spam");
    trash = new EmailFolder("Trash");
    important = new EmailFolder("Important");

    nextEmailId = 1001;
    nextUserId = 1;

    loadData();
    loadSystemConfig(); // Load system configuration
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
    delete undoStack;
    delete redoStack;
    delete scheduledEmails;
    delete incomingEmailQueue;
    delete priorityEmailQueue;
    delete timestampHeap;
    delete systemConfig;
    delete activityLog;
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
    loadAllContactsAndConnections();
    fileHandler->loadSocialGraph(socialGraph);
  }

  void saveData()
  {
    fileHandler->saveAllUsers(users);
    fileHandler->saveSocialGraph(socialGraph);
    saveAllEmails();
    saveAllContactsAndConnections();
  }

  void saveAllContactsAndConnections()
  {
    int maxUsers = 1000;
    string *keys = new string[maxUsers];
    User **values = new User *[maxUsers];

    users->getAllEntries(keys, values, maxUsers);

    for (int i = 0; i < users->getSize(); i++)
    {
      fileHandler->saveUserContacts(values[i]->getEmail(), values[i]->getContacts());

      GraphNode *node = socialGraph->getNode(values[i]->getEmail());
      if (node != nullptr)
      {
        fileHandler->saveUserConnections(values[i]->getEmail(), &node->adjacentUsers, &node->connectionStrengths);
      }
    }

    delete[] keys;
    delete[] values;
  }

  void loadAllContactsAndConnections()
  {
    int maxUsers = 1000;
    string *keys = new string[maxUsers];
    User **values = new User *[maxUsers];

    users->getAllEntries(keys, values, maxUsers);

    for (int i = 0; i < users->getSize(); i++)
    {
      fileHandler->loadUserContacts(values[i]->getEmail(), values[i]->getContacts());

      socialGraph->addUser(values[i]->getEmail());

      LinkedList<string> adjacentUsers;
      LinkedList<int> strengths;
      fileHandler->loadUserConnections(values[i]->getEmail(), &adjacentUsers, &strengths);

      for (int j = 0; j < adjacentUsers.getSize(); j++)
      {
        socialGraph->addConnection(values[i]->getEmail(), adjacentUsers.get(j), strengths.get(j));
      }
    }

    delete[] keys;
    delete[] values;
  }

  void saveAllEmails()
  {
    if (currentUser == nullptr)
      return;

    // Save all emails to user's folder structure
    fileHandler->saveUserEmails(
        currentUser->getEmail(),
        inbox->getEmails(),
        sent->getEmails(),
        drafts->getEmails(),
        spam->getEmails(),
        trash->getEmails(),
        important->getEmails());
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
      // But NOT on sent emails or self-sent emails already in Inbox
      if (email.getReceiver() == currentUser->getEmail() &&
          email.getSender() != currentUser->getEmail() &&
          folder != "Spam" && folder != "Trash")
      {
        if (email.containsSpamWords(spamArr, spamWords->getSize()))
        {
          email.setIsSpam(true);
          email.setFolder("Spam");
          folder = "Spam";
        }
      }

      // Route email to appropriate folder
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

  // Helper function to check if email contains spam
  bool isSpamEmail(const Email &email)
  {
    string spamArr[20];
    for (int i = 0; i < spamWords->getSize(); i++)
    {
      spamArr[i] = spamWords->get(i);
    }
    return email.containsSpamWords(spamArr, spamWords->getSize());
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
      saveData(); // Save all user data
      break;
    case 2:
      newEmail.setFolder("Drafts");
      drafts->addEmail(newEmail);
      cout << "Email saved as draft!" << endl;
      saveData();
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

  void updateEmail(const Email &updatedEmail)
  {
    // Find and update the email in the appropriate folder
    EmailFolder *folder = nullptr;
    string folderName = updatedEmail.getFolder();

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
      // Get all emails from the folder
      LinkedList<Email> *emails = folder->getEmails();

      // Find and update the matching email using iterator
      for (auto it = emails->begin(); it != emails->end(); ++it)
      {
        if ((*it).getEmailId() == updatedEmail.getEmailId())
        {
          *it = updatedEmail;
          // Save the updated email to file
          saveAllEmails();
          break;
        }
      }
    }
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
  Graph *getSocialGraph() { return socialGraph; }
  BST<string, User *> *getUsers() { return users; }

  // Method to deliver email to another user's inbox
  bool deliverEmailToUser(const Email &email, const string &recipientEmail)
  {
    // Check if recipient exists
    User **recipientPtr = users->search(recipientEmail);
    if (recipientPtr == nullptr)
    {
      return false; // User doesn't exist
    }

    // Load recipient's emails
    LinkedList<Email> recipientInbox;
    fileHandler->loadFolderEmails(recipientEmail, "Inbox", &recipientInbox);

    // Create a copy for recipient's inbox
    Email inboxEmail = email;
    inboxEmail.setIsRead(false);

    // Check for spam
    if (isSpamEmail(inboxEmail))
    {
      inboxEmail.setFolder("Spam");
      inboxEmail.setIsSpam(true);

      // Load spam folder instead
      LinkedList<Email> recipientSpam;
      fileHandler->loadFolderEmails(recipientEmail, "Spam", &recipientSpam);
      recipientSpam.insert(inboxEmail);

      // Save spam folder
      LinkedList<Email> empty;
      fileHandler->saveUserEmails(recipientEmail, &empty, &empty, &empty, &recipientSpam, &empty, &empty);
    }
    else
    {
      inboxEmail.setFolder("Inbox");
      recipientInbox.insert(inboxEmail);

      // Save inbox folder
      LinkedList<Email> empty;
      fileHandler->saveUserEmails(recipientEmail, &recipientInbox, &empty, &empty, &empty, &empty, &empty);
    }

    return true;
  }

  // Folder getters for UI
  EmailFolder *getInbox() { return inbox; }
  EmailFolder *getSent() { return sent; }
  EmailFolder *getDrafts() { return drafts; }
  EmailFolder *getSpam() { return spam; }
  EmailFolder *getTrash() { return trash; }
  EmailFolder *getImportant() { return important; }

  // ============= NEW FUNCTIONALITY =============

  // Activity Log (Circular Linked List behavior)
  void logActivity(string activity)
  {
    string logEntry = activity;
    activityLog->insert(logEntry);

    // Keep circular behavior - remove oldest if exceeds max size
    if (activityLog->getSize() > activityLogMaxSize)
    {
      activityLog->removeAt(0);
    }
  }

  void displayActivityLog()
  {
    cout << "\n=== Recent Activity Log ===" << endl;
    for (int i = 0; i < activityLog->getSize(); i++)
    {
      cout << (i + 1) << ". " << activityLog->get(i) << endl;
    }
  }

  // Undo/Redo Functionality
  void undoEmailOperation()
  {
    if (undoStack->isEmpty())
    {
      cout << "Nothing to undo." << endl;
      return;
    }

    Email email = undoStack->pop();
    redoStack->push(email);

    // Remove from current folder and add back to original
    string currentFolder = email.getFolder();
    EmailFolder *folder = getFolderByName(currentFolder);
    if (folder != nullptr)
    {
      folder->removeEmail(email.getEmailId());
    }

    logActivity("Undone operation on email: " + email.getSubject());
    cout << "Operation undone successfully!" << endl;
  }

  void redoEmailOperation()
  {
    if (redoStack->isEmpty())
    {
      cout << "Nothing to redo." << endl;
      return;
    }

    Email email = redoStack->pop();
    undoStack->push(email);

    // Add back to folder
    string folder = email.getFolder();
    EmailFolder *targetFolder = getFolderByName(folder);
    if (targetFolder != nullptr)
    {
      targetFolder->addEmail(email);
    }

    logActivity("Redone operation on email: " + email.getSubject());
    cout << "Operation redone successfully!" << endl;
  }

  // Priority Queue for High-Importance Emails
  void addToPriorityQueue(Email email)
  {
    priorityEmailQueue->enqueue(email, email.getPriority());
    logActivity("Added high-priority email to queue: " + email.getSubject());
  }

  Email getNextPriorityEmail()
  {
    if (priorityEmailQueue->isEmpty())
    {
      throw "No priority emails in queue";
    }
    Email email = priorityEmailQueue->dequeue();
    logActivity("Processed priority email: " + email.getSubject());
    return email;
  }

  // Timestamp Heap Organization
  void organizeByTimestamp()
  {
    cout << "\n=== Organizing Emails by Timestamp ===" << endl;

    // Add all inbox emails to heap
    LinkedList<Email> *emails = inbox->getEmails();
    for (int i = 0; i < emails->getSize(); i++)
    {
      timestampHeap->insert(emails->get(i));
    }

    // Extract and display in order
    cout << "Emails in chronological order (most recent first):" << endl;
    int count = 0;
    while (!timestampHeap->isEmpty() && count < 10)
    {
      Email email = timestampHeap->extractMax();
      cout << ++count << ". " << email.getSubject() << " - From: " << email.getSender() << endl;
    }
  }

  // Scheduled Email Processing
  void processScheduledEmails()
  {
    if (scheduledEmails->isEmpty())
    {
      cout << "No scheduled emails to process." << endl;
      return;
    }

    cout << "\n=== Processing Scheduled Emails ===" << endl;
    while (!scheduledEmails->isEmpty())
    {
      Email email = scheduledEmails->dequeue();
      email.setFolder("Sent");
      sent->addEmail(email);

      // Deliver to recipient
      deliverEmailToUser(email, email.getReceiver());

      logActivity("Sent scheduled email: " + email.getSubject());
      cout << "Scheduled email sent to " << email.getReceiver() << endl;
    }

    saveAllEmails();
  }

  // Incoming Email Queue Processing
  void processIncomingEmails()
  {
    if (incomingEmailQueue->isEmpty())
    {
      cout << "No incoming emails to process." << endl;
      return;
    }

    cout << "\n=== Processing Incoming Emails ===" << endl;
    while (!incomingEmailQueue->isEmpty())
    {
      Email email = incomingEmailQueue->dequeue();

      // Check for spam
      if (isSpamEmail(email))
      {
        email.setFolder("Spam");
        email.setIsSpam(true);
        spam->addEmail(email);
        logActivity("Filtered spam email: " + email.getSubject());
      }
      else
      {
        email.setFolder("Inbox");
        inbox->addEmail(email);
        logActivity("Received email: " + email.getSubject());

        // Add to priority queue if high priority
        if (email.getPriority() >= 3)
        {
          addToPriorityQueue(email);
        }
      }
    }

    saveAllEmails();
  }

  void addToIncomingQueue(Email email)
  {
    incomingEmailQueue->enqueue(email);
  }

  // System Configuration Management
  void loadSystemConfig()
  {
    systemConfig->add("AutoSaveInterval=300");
    systemConfig->add("MaxInboxSize=1000");
    systemConfig->add("SpamFilterEnabled=true");
    systemConfig->add("AutoDeleteTrash=false");
    systemConfig->add("EnableNotifications=true");
    systemConfig->add("Theme=Dark");
    systemConfig->add("FontSize=14");
    systemConfig->add("Language=English");
  }

  void displaySystemConfig()
  {
    cout << "\n=== System Configuration ===" << endl;
    for (int i = 0; i < systemConfig->getSize(); i++)
    {
      cout << (i + 1) << ". " << systemConfig->get(i) << endl;
    }
  }

  string getConfigValue(string key)
  {
    for (int i = 0; i < systemConfig->getSize(); i++)
    {
      string config = systemConfig->get(i);
      if (config.find(key) != string::npos)
      {
        size_t pos = config.find('=');
        if (pos != string::npos)
        {
          return config.substr(pos + 1);
        }
      }
    }
    return "";
  }

  // Helper method to get folder by name
  EmailFolder *getFolderByName(string folderName)
  {
    if (folderName == "Inbox")
      return inbox;
    else if (folderName == "Sent")
      return sent;
    else if (folderName == "Drafts")
      return drafts;
    else if (folderName == "Spam")
      return spam;
    else if (folderName == "Trash")
      return trash;
    else if (folderName == "Important")
      return important;
    return nullptr;
  }

  // Enhanced delete with undo support
  void deleteEmailWithUndo(string emailId, string folderName)
  {
    EmailFolder *folder = getFolderByName(folderName);
    if (folder != nullptr)
    {
      try
      {
        Email email = folder->removeEmail(emailId);
        undoStack->push(email); // Save for undo
        email.setFolder("Trash");
        trash->addEmail(email);
        deletedEmailsStack->push(email);
        logActivity("Deleted email: " + email.getSubject());
        cout << "Email moved to trash. (Undo available)" << endl;
      }
      catch (const char *msg)
      {
        cout << msg << endl;
      }
    }
  }

  // Get statistics
  int getScheduledEmailCount() { return scheduledEmails->getSize(); }
  int getIncomingQueueCount() { return incomingEmailQueue->getSize(); }
  int getPriorityQueueCount() { return priorityEmailQueue->getSize(); }
  bool canUndo() { return !undoStack->isEmpty(); }
  bool canRedo() { return !redoStack->isEmpty(); }
};

#endif

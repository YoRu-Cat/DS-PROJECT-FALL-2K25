#ifndef EMAILFOLDER_H
#define EMAILFOLDER_H

#include <iostream>
#include "LinkedList.h"
#include "Heap.h"
#include "Stack.h"
#include "Email.h"
using namespace std;

class EmailFolder
{
private:
  string folderName;
  LinkedList<Email> *emails;
  MaxHeap<Email> *priorityHeap;
  Stack<Email> *recentEmails;
  int maxRecentSize;

public:
  EmailFolder(string name = "Inbox")
  {
    folderName = name;
    emails = new LinkedList<Email>();
    priorityHeap = new MaxHeap<Email>(100);
    recentEmails = new Stack<Email>();
    maxRecentSize = 10;
  }

  ~EmailFolder()
  {
    delete emails;
    delete priorityHeap;
    delete recentEmails;
  }

  string getFolderName() const { return folderName; }

  void addEmail(Email newEmail)
  {
    emails->insert(newEmail);
    priorityHeap->insert(newEmail);

    // Maintain recent emails stack
    recentEmails->push(newEmail);
  }

  Email removeEmail(string emailId)
  {
    for (int i = 0; i < emails->getSize(); i++)
    {
      Email email = emails->get(i);
      if (email.getEmailId() == emailId)
      {
        emails->removeAt(i);
        return email;
      }
    }
    throw "Email not found";
  }

  bool findEmail(string emailId, Email &result)
  {
    for (int i = 0; i < emails->getSize(); i++)
    {
      Email email = emails->get(i);
      if (email.getEmailId() == emailId)
      {
        result = email;
        return true;
      }
    }
    return false;
  }

  Email getRecentEmail()
  {
    if (!recentEmails->isEmpty())
    {
      return recentEmails->peek();
    }
    throw "No recent emails";
  }

  void displayAllEmails()
  {
    cout << "\n======== " << folderName << " Folder ========" << endl;
    if (emails->isEmpty())
    {
      cout << "No emails in this folder." << endl;
      return;
    }

    for (int i = 0; i < emails->getSize(); i++)
    {
      Email email = emails->get(i);
      cout << "\n"
           << (i + 1) << ". ";
      cout << (email.getIsRead() ? "[READ] " : "[UNREAD] ");
      cout << "From: " << email.getSender() << endl;
      cout << "   Subject: " << email.getSubject() << endl;
      cout << "   Priority: " << email.getPriority() << endl;
    }
    cout << "\nTotal emails: " << emails->getSize() << endl;
  }

  void displayEmailsByPriority()
  {
    cout << "\n======== " << folderName << " (Sorted by Priority) ========" << endl;

    // Create temporary heap copy
    MaxHeap<Email> tempHeap(100);
    for (int i = 0; i < emails->getSize(); i++)
    {
      tempHeap.insert(emails->get(i));
    }

    int count = 1;
    while (!tempHeap.isEmpty())
    {
      Email email = tempHeap.extractMax();
      cout << "\n"
           << count++ << ". ";
      cout << (email.getIsRead() ? "[READ] " : "[UNREAD] ");
      cout << "From: " << email.getSender() << endl;
      cout << "   Subject: " << email.getSubject() << endl;
      cout << "   Priority: " << email.getPriority() << endl;
    }
  }

  int getEmailCount() const
  {
    return emails->getSize();
  }

  int getUnreadCount() const
  {
    int count = 0;
    for (int i = 0; i < emails->getSize(); i++)
    {
      if (!emails->get(i).getIsRead())
      {
        count++;
      }
    }
    return count;
  }

  void markAllAsRead()
  {
    LinkedList<Email> *newList = new LinkedList<Email>();
    for (int i = 0; i < emails->getSize(); i++)
    {
      Email email = emails->get(i);
      email.markAsRead();
      newList->insert(email);
    }
    delete emails;
    emails = newList;
  }

  void clearFolder()
  {
    emails->clear();
    priorityHeap->clear();
    recentEmails->clear();
  }

  LinkedList<Email> *getEmails() { return emails; }
};

#endif

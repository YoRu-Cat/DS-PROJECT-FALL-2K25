#ifndef USER_H
#define USER_H

#include <iostream>
#include <sstream>
#include <ctime>
#include "BST.h"
#include "Contact.h"
#include "Array.h"
using namespace std;

class User
{
private:
  string userId;
  string username;
  string email;
  string password;
  time_t createdDate;
  time_t lastLogin;
  BST<string, Contact> *contacts;
  Array<string> *recentContacts; // Last 10 contacted users

public:
  User()
  {
    userId = "";
    username = "";
    email = "";
    password = "";
    createdDate = time(0);
    lastLogin = time(0);
    contacts = new BST<string, Contact>();
    recentContacts = new Array<string>(10);
  }

  User(string id, string uname, string mail, string pass)
  {
    userId = id;
    username = uname;
    email = mail;
    password = pass;
    createdDate = time(0);
    lastLogin = time(0);
    contacts = new BST<string, Contact>();
    recentContacts = new Array<string>(10);
  }

  ~User()
  {
    delete contacts;
    delete recentContacts;
  }

  // Getters
  string getUserId() const { return userId; }
  string getUsername() const { return username; }
  string getEmail() const { return email; }
  string getPassword() const { return password; }
  time_t getCreatedDate() const { return createdDate; }
  time_t getLastLogin() const { return lastLogin; }

  // Setters
  void setUsername(string uname) { username = uname; }
  void setEmail(string mail) { email = mail; }
  void setPassword(string pass) { password = pass; }
  void setLastLogin(time_t login) { lastLogin = login; }
  void setCreatedDate(time_t created) { createdDate = created; }

  bool validatePassword(string pass)
  {
    return password == pass;
  }

  void addContact(Contact newContact)
  {
    contacts->insert(newContact.getEmail(), newContact);
  }

  Contact *searchContact(string email)
  {
    return contacts->search(email);
  }

  void removeContact(string email)
  {
    contacts->remove(email);
  }

  void addRecentContact(string contactEmail)
  {
    if (!recentContacts->isFull())
    {
      recentContacts->add(contactEmail);
    }
    else
    {
      // Shift array and add new contact
      for (int i = 0; i < 9; i++)
      {
        recentContacts->set(i, recentContacts->get(i + 1));
      }
      recentContacts->set(9, contactEmail);
    }
  }

  void displayRecentContacts()
  {
    cout << "\n=== Recent Contacts ===" << endl;
    for (int i = 0; i < recentContacts->getSize(); i++)
    {
      cout << (i + 1) << ". " << recentContacts->get(i) << endl;
    }
  }

  void displayAllContacts()
  {
    cout << "\\n=== All Contacts ===" << endl;
    if (contacts->isEmpty())
    {
      cout << "No contacts found." << endl;
      return;
    }

    int maxContacts = 1000;
    string *keys = new string[maxContacts];
    Contact *values = new Contact[maxContacts];

    contacts->getAllEntries(keys, values, maxContacts);

    for (int i = 0; i < contacts->getSize(); i++)
    {
      cout << (i + 1) << ". ";
      values[i].display();
    }

    delete[] keys;
    delete[] values;
  }

  string toString() const
  {
    stringstream ss;
    ss << userId << "," << username << "," << email << ","
       << password << "," << createdDate << "," << lastLogin;
    return ss.str();
  }

  void display() const
  {
    cout << "\n=== User Profile ===" << endl;
    cout << "User ID: " << userId << endl;
    cout << "Username: " << username << endl;
    cout << "Email: " << email << endl;
    cout << "Created: " << ctime(&createdDate);
    cout << "Last Login: " << ctime(&lastLogin);
    cout << "===================" << endl;
  }

  BST<string, Contact> *getContacts() { return contacts; }
  Array<string> *getRecentContactsArray() { return recentContacts; }
};

#endif

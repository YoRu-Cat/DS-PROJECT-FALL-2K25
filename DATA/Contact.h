#ifndef CONTACT_H
#define CONTACT_H

#include <iostream>
using namespace std;

class Contact
{
private:
  string contactId;
  string name;
  string email;
  string phone;
  int interactionCount;

public:
  Contact()
  {
    contactId = "";
    name = "";
    email = "";
    phone = "";
    interactionCount = 0;
  }

  Contact(string id, string n, string e, string p = "")
  {
    contactId = id;
    name = n;
    email = e;
    phone = p;
    interactionCount = 0;
  }

  // Getters
  string getContactId() const { return contactId; }
  string getName() const { return name; }
  string getEmail() const { return email; }
  string getPhone() const { return phone; }
  int getInteractionCount() const { return interactionCount; }

  // Setters
  void setName(string n) { name = n; }
  void setEmail(string e) { email = e; }
  void setPhone(string p) { phone = p; }

  void incrementInteraction() { interactionCount++; }

  void display() const
  {
    cout << "Name: " << name << " | Email: " << email;
    if (phone != "")
      cout << " | Phone: " << phone;
    cout << " | Interactions: " << interactionCount << endl;
  }

  // Comparison operators for BST
  bool operator<(const Contact &other) const
  {
    return email < other.email;
  }

  bool operator>(const Contact &other) const
  {
    return email > other.email;
  }

  bool operator==(const Contact &other) const
  {
    return email == other.email;
  }
};

#endif

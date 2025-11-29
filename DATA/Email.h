#ifndef EMAIL_H
#define EMAIL_H

#include <iostream>
#include <ctime>
#include <sstream>
using namespace std;

class Email
{
private:
  string emailId;
  string sender;
  string receiver;
  string subject;
  string content;
  time_t timestamp;
  bool isRead;
  bool isSpam;
  int priority;
  string folder;

public:
  Email()
  {
    emailId = "";
    sender = "";
    receiver = "";
    subject = "";
    content = "";
    timestamp = time(0);
    isRead = false;
    isSpam = false;
    priority = 0;
    folder = "Inbox";
  }

  Email(string id, string from, string to, string subj, string cont)
  {
    emailId = id;
    sender = from;
    receiver = to;
    subject = subj;
    content = cont;
    timestamp = time(0);
    isRead = false;
    isSpam = false;
    priority = 0;
    folder = "Inbox";
  }

  // Getters
  string getEmailId() const { return emailId; }
  string getSender() const { return sender; }
  string getReceiver() const { return receiver; }
  string getSubject() const { return subject; }
  string getContent() const { return content; }
  time_t getTimestamp() const { return timestamp; }
  bool getIsRead() const { return isRead; }
  bool getIsSpam() const { return isSpam; }
  int getPriority() const { return priority; }
  string getFolder() const { return folder; }

  // Setters
  void setEmailId(string id) { emailId = id; }
  void setSender(string from) { sender = from; }
  void setReceiver(string to) { receiver = to; }
  void setSubject(string subj) { subject = subj; }
  void setContent(string cont) { content = cont; }
  void setTimestamp(time_t ts) { timestamp = ts; }
  void setIsRead(bool read) { isRead = read; }
  void setIsSpam(bool spam) { isSpam = spam; }
  void setPriority(int p) { priority = p; }
  void setFolder(string fld) { folder = fld; }

  void markAsRead() { isRead = true; }
  void markAsUnread() { isRead = false; }

  bool containsSpamWords(string spamWords[], int size) const
  {
    string lowerSubject = subject;
    string lowerContent = content;

    // Convert to lowercase for comparison
    for (size_t i = 0; i < lowerSubject.length(); i++)
    {
      lowerSubject[i] = tolower(lowerSubject[i]);
    }
    for (size_t i = 0; i < lowerContent.length(); i++)
    {
      lowerContent[i] = tolower(lowerContent[i]);
    }

    for (int i = 0; i < size; i++)
    {
      string lowerSpam = spamWords[i];
      for (size_t j = 0; j < lowerSpam.length(); j++)
      {
        lowerSpam[j] = tolower(lowerSpam[j]);
      }

      if (lowerSubject.find(lowerSpam) != string::npos ||
          lowerContent.find(lowerSpam) != string::npos)
      {
        return true;
      }
    }
    return false;
  }

  string toString() const
  {
    stringstream ss;
    ss << emailId << "," << sender << "," << receiver << ","
       << subject << "," << content << "," << timestamp << ","
       << isRead << "," << isSpam << "," << priority << "," << folder;
    return ss.str();
  }

  void display() const
  {
    cout << "\n======== EMAIL ========" << endl;
    cout << "ID: " << emailId << endl;
    cout << "From: " << sender << endl;
    cout << "To: " << receiver << endl;
    cout << "Subject: " << subject << endl;
    cout << "Content: " << content << endl;
    cout << "Priority: " << priority << endl;
    cout << "Status: " << (isRead ? "Read" : "Unread") << endl;
    cout << "Folder: " << folder << endl;
    cout << "======================" << endl;
  }

  // Comparison operators for heap operations
  bool operator>(const Email &other) const
  {
    if (priority != other.priority)
      return priority > other.priority;
    return timestamp > other.timestamp;
  }

  bool operator<(const Email &other) const
  {
    if (priority != other.priority)
      return priority < other.priority;
    return timestamp < other.timestamp;
  }

  bool operator==(const Email &other) const
  {
    return emailId == other.emailId;
  }
};

#endif

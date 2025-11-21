# Email Management System - Like Gmail

## Overview

A comprehensive Email Management System implemented in C++ using custom data structures (no STL containers). The system provides complete email functionality with folder management, spam filtering, contacts, and social connections.

## Data Structures Implemented

1. **Array** - Spam words storage, recent contacts
2. **Stack** - Undo/redo, navigation history, deleted emails recovery
3. **Queue** - Scheduled emails, email processing
4. **Heap (MaxHeap)** - Priority inbox management
5. **Binary Search Tree (BST)** - User lookup, contact management
6. **Linked Lists** - Email folders (singly), email navigation (doubly), activity log (circular)
7. **Graph** - Social connections, spam probability calculation

## Project Structure

````
Email Management System/
├── main.cpp                    # Main application entry point
├── DATA/                       # All data structures and classes
│   ├── Array.h                # Custom array implementation
│   ├── Stack.h                # Custom stack implementation
│   ├── Queue.h                # Custom queue implementation
│   ├── LinkedList.h           # Singly, doubly, and circular linked lists
│   ├── BST.h                  # Binary Search Tree
│   ├── Heap.h                 # Max Heap and Priority Queue
│   ├── Graph.h                # Graph for social connections
│   ├── Email.h                # Email class
│   ├── Contact.h              # Contact class
│   ├── User.h                 # User class
│   ├── EmailFolder.h          # Email folder management
│   ├── FileHandler.h          # File I/O operations
│   └── EmailSystem.h          # Main system controller
├── users.txt                  # User account data
├── email.txt                  # Email records
├── spam_words.txt             # Spam filtering keywords
└── social_graph.txt           # User connection data

## Features

### User Management
- Account creation with validation
- Login/logout functionality
- Password authentication
- User profile management

### Email Operations
- Compose and send emails
- Save drafts
- Schedule emails for later
- View emails in different folders:
  - Inbox (with priority sorting)
  - Sent
  - Drafts
  - Spam
  - Trash
  - Important
- Search emails by sender/subject
- Delete and recover emails
- Empty trash functionality

### Contact Management
- Add contacts to BST
- View all contacts
- Track recent contacts (last 10)
- Contact search functionality

### Social Features
- Build social graph of user connections
- View mutual connections
- Spam probability calculation based on connections
- Connection strength tracking

### Spam Filtering
- Automatic spam detection using keyword matching
- Configurable spam words list
- Graph-based spam probability

## Compilation & Execution

### Windows (Using g++)
```bash
g++ -o email_system.exe main.cpp -I.
email_system.exe
````

### Linux/Mac

```bash
g++ -o email_system main.cpp -I.
./email_system
```

## Sample Test Accounts

### Users

- **Email:** john_doe@lhr.nu.edu.pk | **Password:** password123
- **Email:** sarah_smith@lhr.nu.edu.pk | **Password:** securepass456
- **Email:** admin@lhr.nu.edu.pk | **Password:** adminpass

## Menu Navigation

### Main Menu

1. Create Account
2. Login
3. Exit

### Dashboard (After Login)

1. Compose Email
2. View Inbox
3. View Inbox (Priority Sorted)
4. View Sent Emails
5. View Drafts
6. View Spam
7. View Trash
8. View Important
9. Search Emails
10. Delete Email
11. Recover Last Deleted
12. Empty Trash
13. Manage Contacts
14. Manage Connections
15. System Statistics
16. Logout

## Data File Formats

### users.txt

```
userId,username,email,password,createdDate,lastLogin
```

### email.txt

```
emailId,sender,receiver,subject,content,timestamp,isRead,isSpam,priority,folder
```

### spam_words.txt

```
Winner,Free,Urgent,Claim,Bonus,Limited,Exclusive,Gift,Guaranteed,Profit
```

### social_graph.txt

```
user1,user2,connectionStrength,connectionDate
```

## Implementation Notes

### All Data Structures Are Custom

- No STL containers (vector, map, set, etc.) used
- All implementations from scratch
- Proper memory management with destructors

### Design Patterns

- Modular design with separate classes
- Object-oriented programming principles
- Separation of concerns (data structures in DATA folder)

### Key Algorithms

- BST insert/search/delete with O(log n) average case
- Heap operations for priority management
- Graph traversal for connection analysis
- Spam detection using multiple criteria

## Future Enhancements

- Email threading and conversation view
- Advanced search filters
- Attachment handling
- Email scheduling with specific time
- Multi-language support
- Backup and restore functionality

## Author

24L-0602
YoRu-Cat

## Date

November 2025

```

```

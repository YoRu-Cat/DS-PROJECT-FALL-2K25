#include <iostream>
#include <cstdlib>
#include "DATA/EmailSystem.h"
using namespace std;

void clearScreen()
{
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}

void displayWelcome()
{
  cout << "\n";
  cout << "===============================================" << endl;
  cout << "     EMAIL MANAGEMENT SYSTEM - YORU CONEKT     " << endl;
  cout << "===============================================" << endl;
  cout << endl;
}

void displayMainMenu()
{
  cout << "\n=== MAIN MENU ===" << endl;
  cout << "1. Create Account" << endl;
  cout << "2. Login" << endl;
  cout << "3. Exit" << endl;
  cout << "Choice: ";
}

void displayDashboard()
{
  cout << "\n=== EMAIL DASHBOARD ===" << endl;
  cout << "1.  Compose Email" << endl;
  cout << "2.  View Inbox" << endl;
  cout << "3.  View Inbox (Priority Sorted)" << endl;
  cout << "4.  View Sent Emails" << endl;
  cout << "5.  View Drafts" << endl;
  cout << "6.  View Spam" << endl;
  cout << "7.  View Trash" << endl;
  cout << "8.  View Important" << endl;
  cout << "9.  Search Emails" << endl;
  cout << "10. Delete Email" << endl;
  cout << "11. Recover Last Deleted" << endl;
  cout << "12. Empty Trash" << endl;
  cout << "13. Manage Contacts" << endl;
  cout << "14. Manage Connections" << endl;
  cout << "15. System Statistics" << endl;
  cout << "16. Logout" << endl;
  cout << "Choice: ";
}

void displayContactsMenu()
{
  cout << "\n=== CONTACTS MANAGEMENT ===" << endl;
  cout << "1. Add Contact" << endl;
  cout << "2. View All Contacts" << endl;
  cout << "3. View Recent Contacts" << endl;
  cout << "4. Back to Dashboard" << endl;
  cout << "Choice: ";
}

void displayConnectionsMenu()
{
  cout << "\n=== CONNECTIONS MANAGEMENT ===" << endl;
  cout << "1. Add Connection" << endl;
  cout << "2. View Mutual Connections" << endl;
  cout << "3. Back to Dashboard" << endl;
  cout << "Choice: ";
}

void handleAuthentication(EmailSystem &system)
{
  while (!system.isLoggedIn())
  {
    int choice;
    displayMainMenu();

    if (!(cin >> choice))
    {
      cin.clear();
      cin.ignore(10000, '\n');
      cout << "Invalid input! Please enter a number." << endl;
      continue;
    }

    try
    {
      switch (choice)
      {
      case 1:
      {
        string username, email, password;
        cout << "\n=== Create Account ===" << endl;
        cout << "Username: ";
        cin.ignore();
        getline(cin, username);
        cout << "Email: ";
        getline(cin, email);
        cout << "Password: ";
        getline(cin, password);

        if (username.empty() || email.empty() || password.empty())
        {
          cout << "Error: All fields are required!" << endl;
          break;
        }

        system.createAccount(username, email, password);
        break;
      }
      case 2:
      {
        string email, password;
        cout << "\n=== Login ===" << endl;
        cout << "Email: ";
        cin.ignore();
        getline(cin, email);
        cout << "Password: ";
        getline(cin, password);

        if (email.empty() || password.empty())
        {
          cout << "Error: Email and password are required!" << endl;
          break;
        }

        system.login(email, password);
        break;
      }
      case 3:
        cout << "Thank you for using Email Management System!" << endl;
        exit(0);
      default:
        cout << "Invalid choice!" << endl;
      }
    }
    catch (const char *msg)
    {
      cout << "Error: " << msg << endl;
    }
    catch (...)
    {
      cout << "An error occurred!" << endl;
    }
  }
}

void handleContactsMenu(EmailSystem &system)
{
  int choice;
  do
  {
    displayContactsMenu();

    if (!(cin >> choice))
    {
      cin.clear();
      cin.ignore(10000, '\n');
      cout << "Invalid input! Please enter a number." << endl;
      continue;
    }

    try
    {
      switch (choice)
      {
      case 1:
        system.addContact();
        break;
      case 2:
        system.viewContacts();
        break;
      case 3:
        system.viewRecentContacts();
        break;
      case 4:
        return;
      default:
        cout << "Invalid choice!" << endl;
      }
    }
    catch (const char *msg)
    {
      cout << "Error: " << msg << endl;
    }
    catch (...)
    {
      cout << "An error occurred!" << endl;
    }
  } while (choice != 4);
}

void handleConnectionsMenu(EmailSystem &system)
{
  int choice;
  do
  {
    displayConnectionsMenu();

    if (!(cin >> choice))
    {
      cin.clear();
      cin.ignore(10000, '\n');
      cout << "Invalid input! Please enter a number." << endl;
      continue;
    }

    try
    {
      switch (choice)
      {
      case 1:
      {
        string email;
        cout << "Enter user email to connect: ";
        cin.ignore();
        getline(cin, email);
        if (!email.empty())
        {
          system.addSocialConnection(email);
        }
        else
        {
          cout << "Error: Email cannot be empty!" << endl;
        }
        break;
      }
      case 2:
      {
        string email;
        cout << "Enter user email: ";
        cin.ignore();
        getline(cin, email);
        if (!email.empty())
        {
          system.viewMutualConnections(email);
        }
        else
        {
          cout << "Error: Email cannot be empty!" << endl;
        }
        break;
      }
      case 3:
        return;
      default:
        cout << "Invalid choice!" << endl;
      }
    }
    catch (const char *msg)
    {
      cout << "Error: " << msg << endl;
    }
    catch (...)
    {
      cout << "An error occurred!" << endl;
    }
  } while (choice != 3);
}

void handleDashboard(EmailSystem &system)
{
  int choice;
  do
  {
    displayDashboard();

    if (!(cin >> choice))
    {
      cin.clear();
      cin.ignore(10000, '\n');
      cout << "Invalid input! Please enter a number." << endl;
      continue;
    }

    try
    {
      switch (choice)
      {
      case 1:
        system.composeEmail();
        break;
      case 2:
        system.viewFolder("Inbox");
        break;
      case 3:
        system.viewInboxByPriority();
        break;
      case 4:
        system.viewFolder("Sent");
        break;
      case 5:
        system.viewFolder("Drafts");
        break;
      case 6:
        system.viewFolder("Spam");
        break;
      case 7:
        system.viewFolder("Trash");
        break;
      case 8:
        system.viewFolder("Important");
        break;
      case 9:
        system.searchEmail();
        break;
      case 10:
      {
        string emailId, folder;
        cout << "Enter email ID: ";
        cin >> emailId;
        cout << "Enter folder name: ";
        cin >> folder;
        system.deleteEmail(emailId, folder);
        break;
      }
      case 11:
        system.recoverLastDeleted();
        break;
      case 12:
        system.emptyTrash();
        break;
      case 13:
        handleContactsMenu(system);
        break;
      case 14:
        handleConnectionsMenu(system);
        break;
      case 15:
        system.displaySystemStats();
        break;
      case 16:
        system.logout();
        cout << "Logged out successfully!" << endl;
        return;
      default:
        cout << "Invalid choice!" << endl;
      }
    }
    catch (const char *msg)
    {
      cout << "Error: " << msg << endl;
    }
    catch (...)
    {
      cout << "An error occurred!" << endl;
    }
  } while (choice != 16);
}

int main()
{
  EmailSystem system;

  displayWelcome();

  while (true)
  {
    handleAuthentication(system);
    handleDashboard(system);
  }

  return 0;
}

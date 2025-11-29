#ifndef EMAILUI_H
#define EMAILUI_H

#include "raylib.h"
#include "UIComponents.h"
#include "BackgroundManager.h"
#include "../DATA/EmailSystem.h"
#include <string>
#include <vector>

enum class Screen
{
  LOGIN,
  REGISTER,
  MAIN_DASHBOARD,
  INBOX,
  SENT,
  DRAFTS,
  SPAM,
  TRASH,
  IMPORTANT,
  COMPOSE,
  EMAIL_DETAIL,
  CONTACTS,
  SETTINGS,
  STATS
};

class EmailUI
{
private:
  EmailSystem *emailSystem;
  Screen currentScreen;
  Screen previousScreen;

  // UI Components
  Panel *mainPanel;
  Sidebar *sidebar;
  ScrollableList *emailList;
  Modal *messageModal;

  // Login/Register screen
  TextBox *emailInput;
  TextBox *passwordInput;
  TextBox *usernameInput;
  Button *loginButton;
  Button *registerButton;
  Button *switchModeButton;

  // Main dashboard
  Button *composeButton;
  Button *refreshButton;
  Button *logoutButton;
  Button *searchButton;
  TextBox *searchInput;
  Button *inboxButton;
  Button *sentButton;
  Button *contactsButton;
  Button *statsButton;

  // Compose email
  TextBox *toInput;
  TextBox *subjectInput;
  TextBox *contentInput;
  Button *sendButton;
  Button *draftButton;
  Button *cancelButton;
  Button *priorityButtons[6];
  int selectedPriority;

  // Email detail
  Button *backButton;
  Button *deleteButton;
  Button *replyButton;
  Button *markImportantButton;
  Button *markSpamButton;

  // Contacts
  Button *addContactButton;
  std::vector<Button *> contactButtons;

  // Navigation
  Button *backToDashboardButton;

  // Background Manager
  BackgroundManager *backgroundManager;
  int currentBackgroundIndex;
  Button *changeBgButton;

  // State variables
  std::string selectedEmailId;
  Email *currentEmail;
  std::string statusMessage;
  float statusMessageTime;
  std::vector<Email> displayedEmails;
  bool isComposingReply;

  // Screen dimensions
  float screenWidth;
  float screenHeight;
  float sidebarWidth;

public:
  EmailUI(int width, int height);
  ~EmailUI();

  void Initialize();
  void Update(float deltaTime);
  void Draw();

  // Screen management
  void SetScreen(Screen screen);
  Screen GetCurrentScreen() { return currentScreen; }

  // Screen renderers
  void DrawLoginScreen();
  void DrawRegisterScreen();
  void DrawMainDashboard();
  void DrawInboxScreen();
  void DrawComposeScreen();
  void DrawEmailDetailScreen();
  void DrawContactsScreen();
  void DrawStatsScreen();

  // Screen updaters
  void UpdateLoginScreen();
  void UpdateRegisterScreen();
  void UpdateMainDashboard();
  void UpdateInboxScreen();
  void UpdateComposeScreen();
  void UpdateEmailDetailScreen();
  void UpdateContactsScreen();
  void UpdateStatsScreen();

  // Email operations
  void LoadEmails(const char *folderName);
  void SendEmail();
  void SaveDraft();
  void DeleteEmail();
  void ReplyToEmail();
  void MarkAsImportant();
  void MarkAsSpam();
  void SearchEmails(const char *query);

  // UI helpers
  void ShowMessage(const char *message);
  void DrawHeader(const char *title);
  void DrawEmailList(EmailFolder *folder);
  void DrawEmailPreview(Email &email, float y);
  void ClearInputs();

  // Utility
  std::string FormatTime(time_t timestamp);
  std::string TruncateText(const std::string &text, int maxLength);
  Color GetPriorityColor(int priority);

  bool IsRunning() { return !WindowShouldClose(); }
};

#endif // EMAILUI_H

#include "EmailUI.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstring>

EmailUI::EmailUI(int width, int height)
{
  screenWidth = width;
  screenHeight = height;
  sidebarWidth = 250;

  emailSystem = new EmailSystem();
  currentScreen = Screen::LOGIN;
  previousScreen = Screen::LOGIN;

  // Initialize background manager
  backgroundManager = new BackgroundManager(width, height);
  backgroundManager->SetOverlay({10, 15, 30, 255}, 0.5f);
  currentBackgroundIndex = 1;
  // Try to load background - will show solid color if fails
  backgroundManager->LoadBackground("resources/3.png");

  changeBgButton = nullptr;
  mainPanel = nullptr;
  sidebar = nullptr;
  emailList = nullptr;
  messageModal = nullptr;

  emailInput = nullptr;
  passwordInput = nullptr;
  usernameInput = nullptr;
  loginButton = nullptr;
  registerButton = nullptr;
  switchModeButton = nullptr;

  composeButton = nullptr;
  refreshButton = nullptr;
  logoutButton = nullptr;
  searchButton = nullptr;
  searchInput = nullptr;
  inboxButton = nullptr;
  sentButton = nullptr;
  contactsButton = nullptr;
  statsButton = nullptr;

  toInput = nullptr;
  subjectInput = nullptr;
  contentInput = nullptr;
  sendButton = nullptr;
  draftButton = nullptr;
  cancelButton = nullptr;

  for (int i = 0; i < 6; i++)
  {
    priorityButtons[i] = nullptr;
  }
  selectedPriority = 0;

  backButton = nullptr;
  deleteButton = nullptr;
  replyButton = nullptr;
  markImportantButton = nullptr;
  markSpamButton = nullptr;

  addContactButton = nullptr;
  backToDashboardButton = nullptr;

  selectedEmailId = "";
  currentEmail = nullptr;
  statusMessage = "";
  statusMessageTime = 0.0f;
  isComposingReply = false;

  Initialize();
}

EmailUI::~EmailUI()
{
  delete emailSystem;
  delete backgroundManager;

  // Clean up UI components
  delete mainPanel;
  delete sidebar;
  delete emailList;
  delete messageModal;

  delete emailInput;
  delete passwordInput;
  delete usernameInput;
  delete loginButton;
  delete registerButton;
  delete switchModeButton;

  delete composeButton;
  delete refreshButton;
  delete logoutButton;
  delete searchButton;
  delete searchInput;
  delete inboxButton;
  delete sentButton;
  delete contactsButton;
  delete statsButton;

  delete toInput;
  delete subjectInput;
  delete contentInput;
  delete sendButton;
  delete draftButton;
  delete cancelButton;

  for (int i = 0; i < 6; i++)
  {
    delete priorityButtons[i];
  }

  delete backButton;
  delete deleteButton;
  delete replyButton;
  delete markImportantButton;
  delete markSpamButton;

  delete addContactButton;
  delete backToDashboardButton;

  for (auto btn : contactButtons)
  {
    delete btn;
  }
}

void EmailUI::Initialize()
{
  // Initialize login/register components
  float centerX = (screenWidth - 400) / 2;
  float centerY = (screenHeight - 500) / 2;

  emailInput = new TextBox(Rectangle{centerX, centerY + 100, 400, 50}, "Email");
  passwordInput = new TextBox(Rectangle{centerX, centerY + 170, 400, 50}, "Password");
  passwordInput->SetPassword(true);
  usernameInput = new TextBox(Rectangle{centerX, centerY + 100, 400, 50}, "Username");

  loginButton = new Button(Rectangle{centerX, centerY + 250, 190, 50}, "Login", UIColors::PRIMARY);
  registerButton = new Button(Rectangle{centerX + 210, centerY + 250, 190, 50}, "Register", UIColors::SUCCESS);
  switchModeButton = new Button(Rectangle{centerX, centerY + 320, 400, 40}, "Create Account", UIColors::INFO);

  // Initialize sidebar
  sidebar = new Sidebar(Rectangle{0, 0, sidebarWidth, screenHeight});

  // Initialize email list
  emailList = new ScrollableList(Rectangle{sidebarWidth + 20, 120, screenWidth - sidebarWidth - 40, screenHeight - 140}, 85);

  // Initialize modal
  messageModal = new Modal("Message", "");

  // Initialize dashboard buttons
  composeButton = new Button(Rectangle{100, 100, 180, 50}, "Compose", UIColors::SUCCESS);
  refreshButton = new Button(Rectangle{300, 100, 150, 50}, "Refresh", UIColors::INFO);
  logoutButton = new Button(Rectangle{screenWidth - 150, 20, 130, 50}, "Logout", UIColors::DANGER);
  searchInput = new TextBox(Rectangle{100, 170, 300, 45}, "Search emails...", false);
  searchButton = new Button(Rectangle{410, 170, 100, 45}, "Search", UIColors::PRIMARY);
  inboxButton = new Button(Rectangle{100, 250, 200, 55}, "View Inbox", UIColors::PRIMARY);
  sentButton = new Button(Rectangle{100, 320, 200, 55}, "View Sent", UIColors::INFO);
  contactsButton = new Button(Rectangle{100, 390, 200, 55}, "Contacts", UIColors::WARNING);
  statsButton = new Button(Rectangle{100, 460, 200, 55}, "Statistics", UIColors::INFO);
  changeBgButton = new Button(Rectangle{100, 530, 200, 55}, "Change BG", UIColors::SECONDARY);

  // Initialize compose buttons
  float composeX = sidebarWidth + 40;
  toInput = new TextBox(Rectangle{composeX + 100, 120, 600, 50}, "recipient@email.com");
  subjectInput = new TextBox(Rectangle{composeX + 100, 190, 600, 50}, "Subject");
  contentInput = new TextBox(Rectangle{composeX + 100, 260, 600, 300}, "Write your message...", 1000);

  sendButton = new Button(Rectangle{composeX, 580, 150, 50}, "Send", UIColors::SUCCESS);
  draftButton = new Button(Rectangle{composeX + 160, 580, 150, 50}, "Save Draft", UIColors::INFO);
  cancelButton = new Button(Rectangle{composeX + 320, 580, 150, 50}, "Cancel", UIColors::DANGER);

  // Priority buttons - one row below
  for (int i = 0; i < 6; i++)
  {
    char label[10];
    sprintf(label, "%d", i);
    priorityButtons[i] = new Button(Rectangle{composeX + 100 + (i * 70), 650, 60, 50}, label, UIColors::WARNING);
  }

  // Email detail buttons
  backButton = new Button(Rectangle{sidebarWidth + 20, 20, 120, 50}, "← Back", UIColors::SECONDARY);
  deleteButton = new Button(Rectangle{sidebarWidth + 150, 20, 120, 50}, "Delete", UIColors::DANGER);
  replyButton = new Button(Rectangle{sidebarWidth + 280, 20, 120, 50}, "Reply", UIColors::PRIMARY);
  markImportantButton = new Button(Rectangle{sidebarWidth + 410, 20, 140, 50}, "Important", UIColors::WARNING);
  markSpamButton = new Button(Rectangle{sidebarWidth + 560, 20, 120, 50}, "Spam", UIColors::DANGER);

  // Contacts button - positioned at top-right corner
  addContactButton = new Button(Rectangle{screenWidth - 200, 20, 180, 50}, "+ Add Contact", UIColors::SUCCESS);

  // Back to dashboard button
  backToDashboardButton = new Button(Rectangle{30, 30, 150, 50}, "← Back", UIColors::SECONDARY);
}

void EmailUI::Update(float deltaTime)
{
  // Update status message timer
  if (statusMessageTime > 0)
  {
    statusMessageTime -= deltaTime;
  }

  switch (currentScreen)
  {
  case Screen::LOGIN:
    UpdateLoginScreen();
    break;
  case Screen::REGISTER:
    UpdateRegisterScreen();
    break;
  case Screen::MAIN_DASHBOARD:
  case Screen::INBOX:
  case Screen::SENT:
  case Screen::DRAFTS:
  case Screen::SPAM:
  case Screen::TRASH:
  case Screen::IMPORTANT:
    UpdateMainDashboard();
    break;
  case Screen::COMPOSE:
    UpdateComposeScreen();
    break;
  case Screen::EMAIL_DETAIL:
    UpdateEmailDetailScreen();
    break;
  case Screen::CONTACTS:
    UpdateContactsScreen();
    break;
  case Screen::STATS:
    UpdateStatsScreen();
    break;
  case Screen::SCHEDULED_EMAILS:
    UpdateScheduledEmailsScreen();
    break;
  case Screen::ACTIVITY_LOG:
    UpdateActivityLogScreen();
    break;
  case Screen::SYSTEM_CONFIG:
    UpdateSystemConfigScreen();
    break;
  }

  if (messageModal)
  {
    messageModal->Update();
  }
}

void EmailUI::Draw()
{
  // Draw background
  backgroundManager->DrawFittedBackground();

  switch (currentScreen)
  {
  case Screen::LOGIN:
    DrawLoginScreen();
    break;
  case Screen::REGISTER:
    DrawRegisterScreen();
    break;
  case Screen::MAIN_DASHBOARD:
  case Screen::INBOX:
  case Screen::SENT:
  case Screen::DRAFTS:
  case Screen::SPAM:
  case Screen::TRASH:
  case Screen::IMPORTANT:
    DrawMainDashboard();
    break;
  case Screen::COMPOSE:
    DrawComposeScreen();
    break;
  case Screen::EMAIL_DETAIL:
    DrawEmailDetailScreen();
    break;
  case Screen::CONTACTS:
    DrawContactsScreen();
    break;
  case Screen::STATS:
    DrawStatsScreen();
    break;
  }

  // Draw status message
  if (statusMessageTime > 0)
  {
    Rectangle msgBox = {screenWidth / 2 - 200, 20, 400, 60};
    DrawRectangleRounded(msgBox, 0.2f, 10, UIColors::SUCCESS);
    int textWidth = MeasureText(statusMessage.c_str(), 20);
    DrawTextSpaced(statusMessage.c_str(), screenWidth / 2 - textWidth / 2, 40, 20, UIColors::UI_WHITE);
  }

  if (messageModal)
  {
    messageModal->Draw();
  }
  // Don't call EndDrawing here - handled by main loop
}

void EmailUI::DrawLoginScreen()
{
  // Calculate center position
  float panelWidth = 500;
  float panelHeight = 450;
  float centerX = (screenWidth - panelWidth) / 2;
  float centerY = (screenHeight - panelHeight) / 2;

  // Draw panel background
  Panel panel(Rectangle{centerX, centerY, panelWidth, panelHeight}, UIColors::UI_WHITE);
  panel.Draw();

  // Title
  const char *title = "Email Management System";
  int titleSize = 38;
  int titleWidth = MeasureText(title, titleSize);
  DrawTextShadow(title, (screenWidth - titleWidth) / 2, centerY + 30, titleSize, UIColors::PRIMARY);

  // Subtitle
  const char *subtitle = "Sign in to continue";
  int subtitleWidth = MeasureText(subtitle, 20);
  DrawTextSpaced(subtitle, (screenWidth - subtitleWidth) / 2, centerY + 80, 20, {255, 255, 255, 255});

  // Draw email label
  DrawTextSpaced("Email:", centerX + 50, centerY + 115, 18, {255, 255, 255, 255});

  // Update and draw email input
  float inputY = centerY + 140;
  emailInput->SetPosition(centerX + 50, inputY);
  emailInput->SetSize(panelWidth - 100, 50);
  emailInput->Draw();

  // Draw password label
  DrawTextSpaced("Password:", centerX + 50, inputY + 65, 18, {255, 255, 255, 255});

  // Update and draw password input
  passwordInput->SetPosition(centerX + 50, inputY + 90);
  passwordInput->SetSize(panelWidth - 100, 50);
  passwordInput->Draw();

  // Update and draw login button
  loginButton->SetPosition(centerX + 50, inputY + 170);
  loginButton->SetSize((panelWidth - 120) / 2, 50);
  loginButton->Draw();

  // Update and draw switch mode button
  switchModeButton->SetPosition(centerX + 50 + (panelWidth - 120) / 2 + 20, inputY + 170);
  switchModeButton->SetSize((panelWidth - 120) / 2, 50);
  switchModeButton->SetText("Register");
  switchModeButton->Draw();
}
void EmailUI::DrawRegisterScreen()
{
  // Calculate center position
  float panelWidth = 500;
  float panelHeight = 550;
  float centerX = (screenWidth - panelWidth) / 2;
  float centerY = (screenHeight - panelHeight) / 2;

  // Draw panel
  Panel panel(Rectangle{centerX, centerY, panelWidth, panelHeight}, UIColors::UI_WHITE);
  panel.Draw();

  // Title
  const char *title = "Create Account";
  int titleSize = 38;
  int titleWidth = MeasureText(title, titleSize);
  DrawTextShadow(title, (screenWidth - titleWidth) / 2, centerY + 30, titleSize, UIColors::SUCCESS);

  // Subtitle
  const char *subtitle = "Join our email platform";
  int subtitleWidth = MeasureText(subtitle, 20);
  DrawTextSpaced(subtitle, (screenWidth - subtitleWidth) / 2, centerY + 80, 20, {255, 255, 255, 255});

  // Update input positions
  float inputY = centerY + 130;

  // Username
  DrawTextSpaced("Username:", centerX + 50, inputY - 5, 18, {255, 255, 255, 255});
  usernameInput->SetPosition(centerX + 50, inputY + 20);
  usernameInput->SetSize(panelWidth - 100, 50);
  usernameInput->Draw();

  // Email
  DrawTextSpaced("Email:", centerX + 50, inputY + 85, 18, {255, 255, 255, 255});
  emailInput->SetPosition(centerX + 50, inputY + 110);
  emailInput->SetSize(panelWidth - 100, 50);
  emailInput->Draw();

  // Password
  DrawTextSpaced("Password:", centerX + 50, inputY + 175, 18, {255, 255, 255, 255});
  passwordInput->SetPosition(centerX + 50, inputY + 200);
  passwordInput->SetSize(panelWidth - 100, 50);
  passwordInput->Draw();

  // Buttons
  registerButton->SetPosition(centerX + 50, inputY + 280);
  registerButton->SetSize((panelWidth - 120) / 2, 50);
  registerButton->Draw();

  switchModeButton->SetPosition(centerX + 50 + (panelWidth - 120) / 2 + 20, inputY + 280);
  switchModeButton->SetSize((panelWidth - 120) / 2, 50);
  switchModeButton->SetText("Back to Login");
  switchModeButton->Draw();
}

void EmailUI::DrawMainDashboard()
{
  // Top Menu Bar - Full width - semi-transparent white
  float menuBarHeight = 70;
  DrawRectangle(0, 0, screenWidth, menuBarHeight, {255, 255, 255, 40});
  DrawRectangleLinesEx(Rectangle{0, 0, (float)screenWidth, menuBarHeight}, 2, {255, 255, 255, 180});

  // App Title
  DrawTextSpaced("Email Management System", 30, 20, 30, {255, 255, 255, 255});

  // Logout button in top right
  logoutButton->SetPosition(screenWidth - 150, 15);
  logoutButton->SetSize(130, 40);
  logoutButton->Draw();

  // Main content area - centered
  float contentWidth = screenWidth * 0.85f;
  float contentHeight = screenHeight - menuBarHeight - 40;
  float contentX = (screenWidth - contentWidth) / 2;
  float contentY = menuBarHeight + 20;

  // Left panel - Action buttons (6 email functions)
  float leftPanelWidth = 280;
  Panel leftPanel(Rectangle{contentX, contentY, leftPanelWidth, contentHeight}, UIColors::UI_WHITE);
  leftPanel.Draw();

  // Draw section title
  DrawTextSpaced("Email Actions", contentX + 20, contentY + 20, 24, {255, 255, 255, 255});
  DrawLine(contentX + 20, contentY + 52, contentX + leftPanelWidth - 20, contentY + 52, UIColors::BORDER);

  // 6 Email Function Buttons - vertically stacked
  float btnY = contentY + 70;
  float btnSpacing = 75;
  float btnWidth = leftPanelWidth - 40;
  float btnHeight = 55;

  // Button 1: Compose Email
  composeButton->SetPosition(contentX + 20, btnY);
  composeButton->SetSize(btnWidth, btnHeight);
  composeButton->SetText("Compose Email");
  composeButton->Draw();

  // Button 2: View Inbox
  inboxButton->SetPosition(contentX + 20, btnY + btnSpacing);
  inboxButton->SetSize(btnWidth, btnHeight);
  inboxButton->SetText("View Inbox");
  inboxButton->Draw();

  // Button 3: View Sent
  sentButton->SetPosition(contentX + 20, btnY + btnSpacing * 2);
  sentButton->SetSize(btnWidth, btnHeight);
  sentButton->SetText("View Sent");
  sentButton->Draw();

  // Button 4: Search Emails
  searchButton->SetPosition(contentX + 20, btnY + btnSpacing * 3);
  searchButton->SetSize(btnWidth, btnHeight);
  searchButton->SetText("Search Emails");
  searchButton->Draw();

  // Button 5: Manage Contacts
  contactsButton->SetPosition(contentX + 20, btnY + btnSpacing * 4);
  contactsButton->SetSize(btnWidth, btnHeight);
  contactsButton->SetText("Contacts");
  contactsButton->Draw();

  // Button 6: View Stats
  statsButton->SetPosition(contentX + 20, btnY + btnSpacing * 5);
  statsButton->SetSize(btnWidth, btnHeight);
  statsButton->SetText("Statistics");
  statsButton->Draw();

  // Button 7: Change Background
  changeBgButton->SetPosition(contentX + 20, btnY + btnSpacing * 6);
  changeBgButton->SetSize(btnWidth, btnHeight);
  char bgText[30];
  sprintf(bgText, "Change BG (%d/7)", currentBackgroundIndex);
  changeBgButton->SetText(bgText);
  changeBgButton->Draw();

  // Right panel - Message List
  float rightPanelX = contentX + leftPanelWidth + 20;
  float rightPanelWidth = contentWidth - leftPanelWidth - 20;
  Panel rightPanel(Rectangle{rightPanelX, contentY, rightPanelWidth, contentHeight}, UIColors::UI_WHITE);
  rightPanel.Draw();

  // Message list header
  const char *folderTitle = "Inbox";
  switch (currentScreen)
  {
  case Screen::INBOX:
    folderTitle = "Inbox";
    break;
  case Screen::SENT:
    folderTitle = "Sent";
    break;
  case Screen::DRAFTS:
    folderTitle = "Drafts";
    break;
  case Screen::SPAM:
    folderTitle = "Spam";
    break;
  case Screen::TRASH:
    folderTitle = "Trash";
    break;
  case Screen::IMPORTANT:
    folderTitle = "Important";
    break;
  default:
    break;
  }

  DrawTextSpaced(folderTitle, rightPanelX + 20, contentY + 20, 28, {255, 255, 255, 255});

  // Refresh button
  refreshButton->SetPosition(rightPanelX + rightPanelWidth - 140, contentY + 15);
  refreshButton->SetSize(120, 40);
  refreshButton->Draw();

  DrawLine(rightPanelX + 20, contentY + 65, rightPanelX + rightPanelWidth - 20, contentY + 65, UIColors::BORDER);

  // Message list area
  float listY = contentY + 80;
  float listHeight = contentHeight - 95;

  emailList->SetPosition(rightPanelX + 20, listY);
  emailList->SetSize(rightPanelWidth - 40, listHeight);

  emailList->Update(displayedEmails.size());
  emailList->BeginScissorMode();

  int firstVisible = emailList->GetFirstVisibleItem();
  int lastVisible = emailList->GetLastVisibleItem();

  for (int i = firstVisible; i < std::min(lastVisible, (int)displayedEmails.size()); i++)
  {
    Email &email = displayedEmails[i];
    float y = listY + (i * 85) - emailList->GetScrollOffset();

    std::string preview = TruncateText(email.getContent(), 60);
    std::string timeStr = FormatTime(email.getTimestamp());

    EmailListItem item(
        Rectangle{rightPanelX + 20, y, rightPanelWidth - 40, 80},
        email.getSender().c_str(),
        email.getSubject().c_str(),
        preview.c_str(),
        timeStr.c_str(),
        email.getIsRead(),
        false,
        email.getPriority());

    item.Update();
    item.Draw();

    if (item.IsClicked())
    {
      selectedEmailId = email.getEmailId();
      currentEmail = &displayedEmails[i];

      // Mark email as read when clicked
      if (!currentEmail->getIsRead())
      {
        currentEmail->markAsRead();
        // Update the email in the email system and save to file
        emailSystem->updateEmail(*currentEmail);
      }

      SetScreen(Screen::EMAIL_DETAIL);
    }
  }

  emailList->EndScissorMode();

  // No emails message
  if (displayedEmails.empty())
  {
    const char *msg = "No emails to display";
    int msgWidth = MeasureText(msg, 24);
    DrawTextSpaced(msg, rightPanelX + (rightPanelWidth - msgWidth) / 2, contentY + contentHeight / 2, 24, Color{150, 150, 150, 255});
  }
}

void EmailUI::DrawComposeScreen()
{
  // Draw sidebar with semi-transparent white background
  const char *menuItems[] = {"Inbox", "Sent", "Drafts", "Spam", "Trash", "Important", "Contacts", "Stats"};
  sidebar->Draw(menuItems, 8);

  // Header - semi-transparent white
  DrawRectangle(sidebarWidth, 0, screenWidth - sidebarWidth, 90, {255, 255, 255, 40});
  DrawRectangleLinesEx(Rectangle{(float)sidebarWidth, 0, (float)(screenWidth - sidebarWidth), 90}, 2, {255, 255, 255, 180});

  // Title - white text on semi-transparent background
  DrawTextSpaced("Compose Email", sidebarWidth + 40, 30, 36, {255, 255, 255, 255});
  DrawLine(sidebarWidth + 20, 80, screenWidth - 20, 80, UIColors::BORDER);

  // Labels and inputs
  DrawTextSpaced("To:", sidebarWidth + 40, 135, 20, {255, 255, 255, 255});
  toInput->Draw();

  DrawTextSpaced("Subject:", sidebarWidth + 40, 205, 20, {255, 255, 255, 255});
  subjectInput->Draw();

  DrawTextSpaced("Message:", sidebarWidth + 40, 275, 20, {255, 255, 255, 255});
  contentInput->Draw();

  // Priority selection
  DrawTextSpaced("Priority:", sidebarWidth + 40, 665, 20, {255, 255, 255, 255});
  for (int i = 0; i < 6; i++)
  {
    if (i == selectedPriority)
    {
      priorityButtons[i]->SetColor(UIColors::DANGER);
    }
    else
    {
      priorityButtons[i]->SetColor(UIColors::WARNING);
    }
    priorityButtons[i]->Draw();
  }

  // Action buttons
  sendButton->Draw();
  draftButton->Draw();
  cancelButton->Draw();
}

void EmailUI::DrawEmailDetailScreen()
{
  if (currentEmail == nullptr)
  {
    SetScreen(Screen::INBOX);
    return;
  }

  // Draw sidebar
  const char *menuItems[] = {"Inbox", "Sent", "Drafts", "Spam", "Trash", "Important", "Contacts", "Stats"};
  sidebar->Draw(menuItems, 8);

  // Top buttons header - semi-transparent white
  DrawRectangle(sidebarWidth, 0, screenWidth - sidebarWidth, 90, {255, 255, 255, 40});
  DrawRectangleLinesEx(Rectangle{(float)sidebarWidth, 0, (float)(screenWidth - sidebarWidth), 90}, 2, {255, 255, 255, 180});
  DrawLine(sidebarWidth, 90, screenWidth, 90, UIColors::BORDER);

  backButton->Draw();
  deleteButton->Draw();
  replyButton->Draw();
  markImportantButton->Draw();
  markSpamButton->Draw();

  // Email content panel
  Panel contentPanel(Rectangle{sidebarWidth + 20, 110, screenWidth - sidebarWidth - 40, screenHeight - 130});
  contentPanel.Draw();

  float contentX = sidebarWidth + 40;
  float contentY = 130;

  // Subject
  DrawTextSpaced(currentEmail->getSubject().c_str(), contentX, contentY, 28, UIColors::UI_WHITE);
  contentY += 50;

  // From
  std::string fromText = "From: " + currentEmail->getSender();
  DrawTextSpaced(fromText.c_str(), contentX, contentY, 18, UIColors::UI_WHITE);
  contentY += 30;

  // To
  std::string toText = "To: " + currentEmail->getReceiver();
  DrawTextSpaced(toText.c_str(), contentX, contentY, 18, UIColors::UI_WHITE);
  contentY += 30;

  // Time
  std::string timeText = "Date: " + FormatTime(currentEmail->getTimestamp());
  DrawTextSpaced(timeText.c_str(), contentX, contentY, 18, UIColors::UI_WHITE);
  contentY += 30;

  // Priority
  if (currentEmail->getPriority() > 0)
  {
    std::string priorityText = "Priority: " + std::to_string(currentEmail->getPriority());
    DrawTextSpaced(priorityText.c_str(), contentX, contentY, 18, GetPriorityColor(currentEmail->getPriority()));
    contentY += 30;
  }

  DrawLine(contentX, contentY, screenWidth - sidebarWidth - 60, contentY, UIColors::BORDER);
  contentY += 20;

  // Content
  DrawTextSpaced(currentEmail->getContent().c_str(), contentX, contentY, 18, UIColors::UI_WHITE);
}

void EmailUI::DrawContactsScreen()
{
  // Draw sidebar
  const char *menuItems[] = {"Inbox", "Sent", "Drafts", "Spam", "Trash", "Important", "Contacts", "Stats"};
  sidebar->Draw(menuItems, 8);

  // Header - semi-transparent white
  DrawRectangle(sidebarWidth, 0, screenWidth - sidebarWidth, 90, {255, 255, 255, 40});
  DrawRectangleLinesEx(Rectangle{(float)sidebarWidth, 0, (float)(screenWidth - sidebarWidth), 90}, 2, {255, 255, 255, 180});

  // Title
  DrawTextSpaced("Contacts", sidebarWidth + 40, 30, 36, {255, 255, 255, 255});
  DrawLine(sidebarWidth + 20, 80, screenWidth - 20, 80, UIColors::BORDER);

  addContactButton->Draw();

  DrawTextSpaced("Contact management coming soon...", sidebarWidth + 40, 150, 20, {255, 255, 255, 255});
}

void EmailUI::DrawStatsScreen()
{
  // Draw sidebar
  const char *menuItems[] = {"Inbox", "Sent", "Drafts", "Spam", "Trash", "Important", "Contacts", "Stats"};
  sidebar->Draw(menuItems, 8);

  // Header - semi-transparent white
  DrawRectangle(sidebarWidth, 0, screenWidth - sidebarWidth, 90, {255, 255, 255, 40});
  DrawRectangleLinesEx(Rectangle{(float)sidebarWidth, 0, (float)(screenWidth - sidebarWidth), 90}, 2, {255, 255, 255, 180});

  // Title
  DrawTextSpaced("Statistics", sidebarWidth + 40, 30, 36, {255, 255, 255, 255});
  DrawLine(sidebarWidth + 20, 80, screenWidth - 20, 80, UIColors::BORDER);

  if (emailSystem->isLoggedIn())
  {
    float y = 130;
    float x = sidebarWidth + 60;

    DrawTextSpaced("System Statistics:", x, y, 24, UIColors::UI_WHITE);
    y += 50;

    // Display stats from email system
    DrawTextSpaced("Total emails across all folders", x, y, 20, UIColors::UI_WHITE);
    y += 40;
  }
  else
  {
    DrawTextSpaced("Please log in to view statistics", sidebarWidth + 40, 150, 20, UIColors::TEXT_DARK);
  }
}

void EmailUI::UpdateLoginScreen()
{
  emailInput->Update();
  passwordInput->Update();
  loginButton->Update();
  switchModeButton->Update();

  if (loginButton->IsClicked())
  {
    std::string email = emailInput->GetText();
    std::string password = passwordInput->GetText();

    if (emailSystem->login(email, password))
    {
      // Update last login time and save
      emailSystem->getCurrentUser()->setLastLogin(time(0));
      emailSystem->saveData();

      ShowMessage("Login successful!");
      SetScreen(Screen::INBOX);
      LoadEmails("Inbox");
    }
    else
    {
      ShowMessage("Invalid email or password");
    }
  }

  if (switchModeButton->IsClicked())
  {
    SetScreen(Screen::REGISTER);
    ClearInputs();
  }
}

void EmailUI::UpdateRegisterScreen()
{
  usernameInput->Update();
  emailInput->Update();
  passwordInput->Update();
  registerButton->Update();
  switchModeButton->Update();

  if (registerButton->IsClicked())
  {
    std::string username = usernameInput->GetText();
    std::string email = emailInput->GetText();
    std::string password = passwordInput->GetText();

    if (emailSystem->createAccount(username, email, password))
    {
      ShowMessage("Account created successfully!");
      SetScreen(Screen::LOGIN);
      ClearInputs();
    }
    else
    {
      ShowMessage("Registration failed");
    }
  }

  if (switchModeButton->IsClicked())
  {
    SetScreen(Screen::LOGIN);
    ClearInputs();
  }
}

void EmailUI::UpdateMainDashboard()
{
  composeButton->Update();
  refreshButton->Update();
  logoutButton->Update();
  searchInput->Update();
  searchButton->Update();
  inboxButton->Update();
  sentButton->Update();
  contactsButton->Update();
  statsButton->Update();
  changeBgButton->Update();

  // Sidebar navigation
  const char *menuItems[] = {"Inbox", "Sent", "Drafts", "Spam", "Trash", "Important", "Contacts", "Stats", "Scheduled", "Activity", "Config"};
  int clicked = sidebar->Update(menuItems, 11);

  if (clicked >= 0)
  {
    switch (clicked)
    {
    case 0:
      SetScreen(Screen::INBOX);
      LoadEmails("Inbox");
      break;
    case 1:
      SetScreen(Screen::SENT);
      LoadEmails("Sent");
      break;
    case 2:
      SetScreen(Screen::DRAFTS);
      LoadEmails("Drafts");
      break;
    case 3:
      SetScreen(Screen::SPAM);
      LoadEmails("Spam");
      break;
    case 4:
      SetScreen(Screen::TRASH);
      LoadEmails("Trash");
      break;
    case 5:
      SetScreen(Screen::IMPORTANT);
      LoadEmails("Important");
      break;
    case 6:
      SetScreen(Screen::CONTACTS);
      break;
    case 7:
      SetScreen(Screen::STATS);
      break;
    case 8:
      SetScreen(Screen::SCHEDULED_EMAILS);
      break;
    case 9:
      SetScreen(Screen::ACTIVITY_LOG);
      break;
    case 10:
      SetScreen(Screen::SYSTEM_CONFIG);
      break;
    }
  }

  if (composeButton->IsClicked())
  {
    SetScreen(Screen::COMPOSE);
    ClearInputs();
  }

  if (refreshButton->IsClicked())
  {
    // Reload current folder
    ShowMessage("Refreshed!");
  }

  if (logoutButton->IsClicked())
  {
    emailSystem->logout();
    SetScreen(Screen::LOGIN);
    displayedEmails.clear();
    ClearInputs();
    ShowMessage("Logged out successfully");
  }

  if (searchButton->IsClicked())
  {
    SearchEmails(searchInput->GetText().c_str());
  }

  if (inboxButton->IsClicked())
  {
    SetScreen(Screen::INBOX);
    LoadEmails("Inbox");
  }

  if (sentButton->IsClicked())
  {
    SetScreen(Screen::SENT);
    LoadEmails("Sent");
  }

  if (contactsButton->IsClicked())
  {
    SetScreen(Screen::CONTACTS);
  }

  // Keyboard shortcuts for undo/redo
  if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))
  {
    if (IsKeyPressed(KEY_Z) && emailSystem->canUndo())
    {
      UndoLastOperation();
    }
    if (IsKeyPressed(KEY_Y) && emailSystem->canRedo())
    {
      RedoLastOperation();
    }
  }

  // Draw undo/redo buttons
  Rectangle undoRect = {950, 50, 150, 40};
  bool undoHovered = CheckCollisionPointRec(GetMousePosition(), undoRect);
  DrawRectangleRec(undoRect, emailSystem->canUndo() ? (undoHovered ? UIColors::PRIMARY_HOVER : BLUE) : GRAY);
  DrawTextSpaced("Undo (Ctrl+Z)", 960, 60, 16, WHITE);
  if (undoHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && emailSystem->canUndo())
  {
    UndoLastOperation();
  }

  Rectangle redoRect = {1110, 50, 150, 40};
  bool redoHovered = CheckCollisionPointRec(GetMousePosition(), redoRect);
  DrawRectangleRec(redoRect, emailSystem->canRedo() ? (redoHovered ? UIColors::PRIMARY_HOVER : BLUE) : GRAY);
  DrawTextSpaced("Redo (Ctrl+Y)", 1120, 60, 16, WHITE);
  if (redoHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && emailSystem->canRedo())
  {
    RedoLastOperation();
  }

  if (statsButton->IsClicked())
  {
    SetScreen(Screen::STATS);
  }

  if (changeBgButton->IsClicked())
  {
    currentBackgroundIndex++;
    if (currentBackgroundIndex > 7)
      currentBackgroundIndex = 1;

    char bgPath[50];
    sprintf(bgPath, "resources/%d.png", currentBackgroundIndex);
    backgroundManager->LoadBackground(bgPath);

    char msg[50];
    sprintf(msg, "Background changed to %d", currentBackgroundIndex);
    ShowMessage(msg);
  }
}

void EmailUI::UpdateComposeScreen()
{
  toInput->Update();
  subjectInput->Update();
  contentInput->Update();
  sendButton->Update();
  draftButton->Update();
  cancelButton->Update();

  // Priority button updates
  for (int i = 0; i < 6; i++)
  {
    priorityButtons[i]->Update();
    if (priorityButtons[i]->IsClicked())
    {
      selectedPriority = i;
    }
  }

  if (sendButton->IsClicked())
  {
    SendEmail();
  }

  if (draftButton->IsClicked())
  {
    SaveDraft();
  }

  if (cancelButton->IsClicked())
  {
    SetScreen(Screen::INBOX);
    ClearInputs();
  }
}

void EmailUI::UpdateEmailDetailScreen()
{
  backButton->Update();
  deleteButton->Update();
  replyButton->Update();
  markImportantButton->Update();
  markSpamButton->Update();

  if (backButton->IsClicked())
  {
    SetScreen(previousScreen);
    currentEmail = nullptr;
  }

  if (deleteButton->IsClicked())
  {
    DeleteEmail();
  }

  if (replyButton->IsClicked())
  {
    ReplyToEmail();
  }

  if (markImportantButton->IsClicked())
  {
    MarkAsImportant();
  }

  if (markSpamButton->IsClicked())
  {
    MarkAsSpam();
  }
}

void EmailUI::UpdateContactsScreen()
{
  // Update sidebar navigation
  const char *menuItems[] = {"Inbox", "Sent", "Drafts", "Spam", "Trash", "Important", "Contacts", "Stats"};
  int clicked = sidebar->Update(menuItems, 8);

  if (clicked >= 0)
  {
    switch (clicked)
    {
    case 0:
      SetScreen(Screen::INBOX);
      LoadEmails("Inbox");
      break;
    case 1:
      SetScreen(Screen::SENT);
      LoadEmails("Sent");
      break;
    case 2:
      SetScreen(Screen::DRAFTS);
      LoadEmails("Drafts");
      break;
    case 3:
      SetScreen(Screen::SPAM);
      LoadEmails("Spam");
      break;
    case 4:
      SetScreen(Screen::TRASH);
      LoadEmails("Trash");
      break;
    case 5:
      SetScreen(Screen::IMPORTANT);
      LoadEmails("Important");
      break;
    case 6:
      SetScreen(Screen::CONTACTS);
      break;
    case 7:
      SetScreen(Screen::STATS);
      break;
    }
  }

  addContactButton->Update();

  if (addContactButton->IsClicked())
  {
    // Add contact functionality
    ShowMessage("Add contact functionality");
  }
}

void EmailUI::UpdateStatsScreen()
{
  // Update sidebar navigation
  const char *menuItems[] = {"Inbox", "Sent", "Drafts", "Spam", "Trash", "Important", "Contacts", "Stats"};
  int clicked = sidebar->Update(menuItems, 8);

  if (clicked >= 0)
  {
    switch (clicked)
    {
    case 0:
      SetScreen(Screen::INBOX);
      LoadEmails("Inbox");
      break;
    case 1:
      SetScreen(Screen::SENT);
      LoadEmails("Sent");
      break;
    case 2:
      SetScreen(Screen::DRAFTS);
      LoadEmails("Drafts");
      break;
    case 3:
      SetScreen(Screen::SPAM);
      LoadEmails("Spam");
      break;
    case 4:
      SetScreen(Screen::TRASH);
      LoadEmails("Trash");
      break;
    case 5:
      SetScreen(Screen::IMPORTANT);
      LoadEmails("Important");
      break;
    case 6:
      SetScreen(Screen::CONTACTS);
      break;
    case 7:
      SetScreen(Screen::STATS);
      break;
    }
  }
}

void EmailUI::SetScreen(Screen screen)
{
  previousScreen = currentScreen;
  currentScreen = screen;

  // Update sidebar selection
  if (sidebar)
  {
    switch (screen)
    {
    case Screen::INBOX:
      sidebar->SetSelectedIndex(0);
      break;
    case Screen::SENT:
      sidebar->SetSelectedIndex(1);
      break;
    case Screen::DRAFTS:
      sidebar->SetSelectedIndex(2);
      break;
    case Screen::SPAM:
      sidebar->SetSelectedIndex(3);
      break;
    case Screen::TRASH:
      sidebar->SetSelectedIndex(4);
      break;
    case Screen::IMPORTANT:
      sidebar->SetSelectedIndex(5);
      break;
    case Screen::CONTACTS:
      sidebar->SetSelectedIndex(6);
      break;
    case Screen::STATS:
      sidebar->SetSelectedIndex(7);
      break;
    default:
      break;
    }
  }
}

void EmailUI::LoadEmails(const char *folderName)
{
  displayedEmails.clear();
  currentFolderName = folderName; // Store current folder

  if (!emailSystem->isLoggedIn())
    return;

  // Get emails from the appropriate folder
  EmailFolder *folder = nullptr;

  if (strcmp(folderName, "Inbox") == 0)
    folder = emailSystem->getInbox();
  else if (strcmp(folderName, "Sent") == 0)
    folder = emailSystem->getSent();
  else if (strcmp(folderName, "Drafts") == 0)
    folder = emailSystem->getDrafts();
  else if (strcmp(folderName, "Spam") == 0)
    folder = emailSystem->getSpam();
  else if (strcmp(folderName, "Trash") == 0)
    folder = emailSystem->getTrash();
  else if (strcmp(folderName, "Important") == 0)
    folder = emailSystem->getImportant();

  if (folder)
  {
    LinkedList<Email> *emails = folder->getEmails();
    for (int i = 0; i < emails->getSize(); i++)
    {
      displayedEmails.push_back(emails->get(i));
    }
  }

  char msg[100];
  sprintf(msg, "Loaded %zu emails from %s", displayedEmails.size(), folderName);
  ShowMessage(msg);
}

void EmailUI::SendEmail()
{
  std::string to = toInput->GetText();
  std::string subject = subjectInput->GetText();
  std::string content = contentInput->GetText();

  if (to.empty() || subject.empty())
  {
    ShowMessage("Please fill in recipient and subject");
    return;
  }

  std::string senderEmail = emailSystem->getCurrentUser()->getEmail();

  // Create and send email
  Email newEmail("E" + std::to_string(time(0)),
                 senderEmail,
                 to, subject, content);
  newEmail.setPriority(selectedPriority);

  // Save to sender's Sent folder
  newEmail.setFolder("Sent");
  emailSystem->getSent()->addEmail(newEmail);

  // If sending to self, also add to Inbox with spam check
  if (to == senderEmail)
  {
    Email inboxCopy = newEmail;
    inboxCopy.setIsRead(false);

    // Check for spam
    if (emailSystem->isSpamEmail(inboxCopy))
    {
      inboxCopy.setFolder("Spam");
      inboxCopy.setIsSpam(true);
      emailSystem->getSpam()->addEmail(inboxCopy);
    }
    else
    {
      inboxCopy.setFolder("Inbox");
      emailSystem->getInbox()->addEmail(inboxCopy);
    }
  }
  else
  {
    // Sending to another user - deliver to their inbox
    emailSystem->deliverEmailToUser(newEmail, to);
  }

  // Log activity
  emailSystem->logActivity("Sent email to " + to + ": " + subject);

  // Add to recent contacts
  emailSystem->getCurrentUser()->addRecentContact(to);

  emailSystem->saveAllEmails();

  ShowMessage("Email sent successfully!");
  SetScreen(Screen::SENT);
  LoadEmails("Sent");
  ClearInputs();
}
void EmailUI::SaveDraft()
{
  std::string to = toInput->GetText();
  std::string subject = subjectInput->GetText();
  std::string content = contentInput->GetText();

  if (subject.empty() && content.empty())
  {
    ShowMessage("Draft is empty!");
    return;
  }

  Email draft("E" + std::to_string(time(0)),
              emailSystem->getCurrentUser()->getEmail(),
              to.empty() ? "" : to,
              subject, content);
  draft.setFolder("Drafts");
  draft.setPriority(0);

  emailSystem->getDrafts()->addEmail(draft);
  emailSystem->saveAllEmails();

  ShowMessage("Draft saved!");
  SetScreen(Screen::DRAFTS);
  LoadEmails("Drafts");
  ClearInputs();
}

void EmailUI::DeleteEmail()
{
  if (currentEmail)
  {
    // Use enhanced delete with undo support
    emailSystem->deleteEmailWithUndo(currentEmail->getEmailId(), currentEmail->getFolder());
    emailSystem->saveAllEmails();

    ShowMessage("Email moved to trash (Undo available)");
    SetScreen(previousScreen);

    // Reload current folder
    if (previousScreen == Screen::INBOX)
      LoadEmails("Inbox");
    else if (previousScreen == Screen::SENT)
      LoadEmails("Sent");
    else if (previousScreen == Screen::DRAFTS)
      LoadEmails("Drafts");
    else if (previousScreen == Screen::SPAM)
      LoadEmails("Spam");
    else if (previousScreen == Screen::IMPORTANT)
      LoadEmails("Important");

    currentEmail = nullptr;
  }
}

void EmailUI::ReplyToEmail()
{
  if (currentEmail)
  {
    SetScreen(Screen::COMPOSE);
    toInput->SetText(currentEmail->getSender().c_str());
    std::string replySubject = "RE: " + currentEmail->getSubject();
    subjectInput->SetText(replySubject.c_str());
    isComposingReply = true;
  }
}

void EmailUI::MarkAsImportant()
{
  if (currentEmail)
  {
    currentEmail->setFolder("Important");
    emailSystem->getImportant()->addEmail(*currentEmail);
    emailSystem->saveAllEmails();

    ShowMessage("Marked as important");
  }
}

void EmailUI::MarkAsSpam()
{
  if (currentEmail)
  {
    currentEmail->setIsSpam(true);
    currentEmail->setFolder("Spam");
    emailSystem->getSpam()->addEmail(*currentEmail);
    emailSystem->saveAllEmails();

    ShowMessage("Marked as spam");
    SetScreen(previousScreen);

    // Reload current folder
    if (previousScreen == Screen::INBOX)
      LoadEmails("Inbox");
    else if (previousScreen == Screen::SENT)
      LoadEmails("Sent");

    currentEmail = nullptr;
  }
}

void EmailUI::SearchEmails(const char *query)
{
  if (strlen(query) == 0)
  {
    ShowMessage("Enter a search term");
    return;
  }

  // Implement search functionality
  // displayedEmails = emailSystem->searchEmails(query);
  ShowMessage("Search completed");
}

void EmailUI::ShowMessage(const char *message)
{
  statusMessage = message;
  statusMessageTime = 3.0f;
}

void EmailUI::DrawHeader(const char *title)
{
  DrawTextSpaced(title, sidebarWidth + 40, 30, 36, UIColors::DARK);
  DrawLine(sidebarWidth + 20, 80, screenWidth - 20, 80, UIColors::BORDER);
}

void EmailUI::ClearInputs()
{
  if (emailInput)
    emailInput->Clear();
  if (passwordInput)
    passwordInput->Clear();
  if (usernameInput)
    usernameInput->Clear();
  if (toInput)
    toInput->Clear();
  if (subjectInput)
    subjectInput->Clear();
  if (contentInput)
    contentInput->Clear();
  if (searchInput)
    searchInput->Clear();
  selectedPriority = 0;
  isComposingReply = false;
}

std::string EmailUI::FormatTime(time_t timestamp)
{
  struct tm *timeinfo = localtime(&timestamp);
  char buffer[80];
  strftime(buffer, sizeof(buffer), "%b %d, %I:%M %p", timeinfo);
  return std::string(buffer);
}

std::string EmailUI::TruncateText(const std::string &text, int maxLength)
{
  if (text.length() <= maxLength)
  {
    return text;
  }
  return text.substr(0, maxLength) + "...";
}

Color EmailUI::GetPriorityColor(int priority)
{
  if (priority >= 4)
    return UIColors::DANGER;
  if (priority >= 2)
    return UIColors::WARNING;
  return UIColors::INFO;
}

// ============= NEW FUNCTIONALITY IMPLEMENTATIONS =============

void EmailUI::UndoLastOperation()
{
  emailSystem->undoEmailOperation();
  LoadEmails(currentFolderName.c_str());
  ShowMessage("Operation undone");
}

void EmailUI::RedoLastOperation()
{
  emailSystem->redoEmailOperation();
  LoadEmails(currentFolderName.c_str());
  ShowMessage("Operation redone");
}

void EmailUI::ProcessScheduledEmails()
{
  emailSystem->processScheduledEmails();
  ShowMessage("Scheduled emails processed");
}

void EmailUI::ProcessIncomingEmails()
{
  emailSystem->processIncomingEmails();
  LoadEmails("Inbox");
  ShowMessage("Incoming emails processed");
}

void EmailUI::UpdateScheduledEmailsScreen()
{
  // Draw title
  DrawTextSpaced("Scheduled Emails", 300, 50, 32, WHITE);

  // Display scheduled email count
  int scheduledCount = emailSystem->getScheduledEmailCount();
  std::string countText = "Scheduled Emails: " + std::to_string(scheduledCount);
  DrawTextSpaced(countText.c_str(), 300, 100, 20, LIGHTGRAY);

  // Process button
  Rectangle processBtn = {300, 150, 250, 50};
  bool processHovered = CheckCollisionPointRec(GetMousePosition(), processBtn);
  DrawRectangleRec(processBtn, processHovered ? UIColors::PRIMARY_HOVER : BLUE);
  DrawTextSpaced("Process Scheduled", 320, 165, 18, WHITE);
  if (processHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
  {
    ProcessScheduledEmails();
  }

  // Back button
  Rectangle backBtn = {300, 220, 250, 50};
  bool backHovered = CheckCollisionPointRec(GetMousePosition(), backBtn);
  DrawRectangleRec(backBtn, backHovered ? DARKGRAY : GRAY);
  DrawTextSpaced("Back to Dashboard", 320, 235, 18, WHITE);
  if (backHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
  {
    SetScreen(Screen::MAIN_DASHBOARD);
  }

  // Display stats
  DrawTextSpaced("Priority Queue:", 300, 300, 20, WHITE);
  std::string priorityText = "High-priority emails: " + std::to_string(emailSystem->getPriorityQueueCount());
  DrawTextSpaced(priorityText.c_str(), 300, 330, 18, LIGHTGRAY);

  DrawTextSpaced("Incoming Queue:", 300, 380, 20, WHITE);
  std::string incomingText = "Pending emails: " + std::to_string(emailSystem->getIncomingQueueCount());
  DrawTextSpaced(incomingText.c_str(), 300, 410, 18, LIGHTGRAY);
}

void EmailUI::UpdateActivityLogScreen()
{
  // Draw title
  DrawTextSpaced("Activity Log", 300, 50, 32, WHITE);

  // Display activity log
  emailSystem->displayActivityLog();

  // Back button
  Rectangle backBtn = {300, 500, 250, 50};
  bool backHovered = CheckCollisionPointRec(GetMousePosition(), backBtn);
  DrawRectangleRec(backBtn, backHovered ? DARKGRAY : GRAY);
  DrawTextSpaced("Back to Dashboard", 320, 515, 18, WHITE);
  if (backHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
  {
    SetScreen(Screen::MAIN_DASHBOARD);
  }
}

void EmailUI::UpdateSystemConfigScreen()
{
  // Draw title
  DrawTextSpaced("System Configuration", 300, 50, 32, WHITE);

  // Display system config
  emailSystem->displaySystemConfig();

  // Back button
  Rectangle backBtn = {300, 500, 250, 50};
  bool backHovered = CheckCollisionPointRec(GetMousePosition(), backBtn);
  DrawRectangleRec(backBtn, backHovered ? DARKGRAY : GRAY);
  DrawTextSpaced("Back to Dashboard", 320, 515, 18, WHITE);
  if (backHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
  {
    SetScreen(Screen::MAIN_DASHBOARD);
  }

  // Display some config values
  std::string theme = "Theme: " + emailSystem->getConfigValue("Theme");
  DrawTextSpaced(theme.c_str(), 300, 150, 18, WHITE);

  std::string spamFilter = "Spam Filter: " + emailSystem->getConfigValue("SpamFilterEnabled");
  DrawTextSpaced(spamFilter.c_str(), 300, 180, 18, WHITE);

  std::string maxInbox = "Max Inbox Size: " + emailSystem->getConfigValue("MaxInboxSize");
  DrawTextSpaced(maxInbox.c_str(), 300, 210, 18, WHITE);
}

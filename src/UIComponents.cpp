#include "UIComponents.h"
#include <cstring>
#include <algorithm>

// Global custom font pointer
Font *globalCustomFont = nullptr;

void SetGlobalFont(Font *font)
{
  globalCustomFont = font;
}

// Helper function for drawing text with letter spacing
void DrawTextSpaced(const char *text, int posX, int posY, int fontSize, Color color, float spacing)
{
  Font fontToUse = (globalCustomFont != nullptr) ? *globalCustomFont : GetFontDefault();
  DrawTextEx(fontToUse, text, {(float)posX, (float)posY}, fontSize, spacing, color);
}

// Helper function implementations
void DrawTextShadow(const char *text, int posX, int posY, int fontSize, Color color)
{
  DrawTextSpaced(text, posX + 2, posY + 2, fontSize, UIColors::SHADOW);
  DrawTextSpaced(text, posX, posY, fontSize, color);
}

void DrawRoundedRectangleShadow(Rectangle rec, float roundness, int segments, Color color)
{
  Rectangle shadowRec = {rec.x + 4, rec.y + 4, rec.width, rec.height};
  DrawRectangleRounded(shadowRec, roundness, segments, UIColors::SHADOW);
  DrawRectangleRounded(rec, roundness, segments, color);
}

// Button implementation
Button::Button()
{
  bounds = {0, 0, 100, 40};
  text = "Button";
  color = UIColors::PRIMARY;
  hoverColor = UIColors::PRIMARY_HOVER;
  textColor = UIColors::UI_WHITE;
  isHovered = false;
  isPressed = false;
  fontSize = 20;
  borderRadius = 0.2f;
}

Button::Button(Rectangle bounds, const char *text, Color color)
{
  this->bounds = bounds;
  this->text = text;
  this->color = color;
  this->hoverColor = Color{
      (unsigned char)std::min(255, color.r + 20),
      (unsigned char)std::min(255, color.g + 20),
      (unsigned char)std::min(255, color.b + 20),
      color.a};
  this->textColor = UIColors::UI_WHITE;
  this->isHovered = false;
  this->isPressed = false;
  this->fontSize = 20;
  this->borderRadius = 0.2f;
}

void Button::Update()
{
  Vector2 mousePos = GetMousePosition();
  isHovered = CheckCollisionPointRec(mousePos, bounds);

  if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
  {
    isPressed = true;
  }
}

void Button::Draw()
{
  Color btnColor = isHovered ? hoverColor : color;

  // Transparent/semi-transparent background (lighter when hovered)
  Color bgColor = {btnColor.r, btnColor.g, btnColor.b, (unsigned char)(isHovered ? 80 : 50)};
  DrawRectangle(bounds.x + 2, bounds.y + 2, bounds.width - 4, bounds.height - 4, bgColor);

  // Draw thick white border with 2px margin inset (game style) - 4px thickness
  Rectangle innerBounds = {bounds.x + 2, bounds.y + 2, bounds.width - 4, bounds.height - 4};
  DrawRectangleLinesEx(innerBounds, 4, {255, 255, 255, (unsigned char)(isHovered ? 255 : 220)});

  // Text in WHITE with proper padding
  int textWidth = MeasureText(text.c_str(), fontSize);
  int textX = bounds.x + (bounds.width - textWidth) / 2;
  int textY = bounds.y + (bounds.height - fontSize) / 2;
  DrawTextSpaced(text.c_str(), textX, textY, fontSize, {255, 255, 255, 255});
}
bool Button::IsClicked()
{
  if (isPressed)
  {
    isPressed = false;
    return true;
  }
  return false;
}

void Button::SetText(const char *text)
{
  this->text = text;
}

void Button::SetPosition(float x, float y)
{
  bounds.x = x;
  bounds.y = y;
}

void Button::SetSize(float width, float height)
{
  bounds.width = width;
  bounds.height = height;
}

void Button::SetColor(Color color)
{
  this->color = color;
}

// TextBox implementation
TextBox::TextBox()
{
  bounds = {0, 0, 200, 40};
  text = "";
  placeholder = "Enter text...";
  isActive = false;
  isPassword = false;
  maxLength = 100;
  fontSize = 20;
  cursorBlinkTime = 0.0f;
  showCursor = true;
}

TextBox::TextBox(Rectangle bounds, const char *placeholder, int maxLength)
{
  this->bounds = bounds;
  this->text = "";
  this->placeholder = placeholder;
  this->isActive = false;
  this->isPassword = false;
  this->maxLength = maxLength;
  this->fontSize = 20;
  this->cursorBlinkTime = 0.0f;
  this->showCursor = true;
}

void TextBox::Update()
{
  Vector2 mousePos = GetMousePosition();

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
  {
    isActive = CheckCollisionPointRec(mousePos, bounds);
  }

  if (isActive)
  {
    // Cursor blink
    cursorBlinkTime += GetFrameTime();
    if (cursorBlinkTime >= 0.5f)
    {
      showCursor = !showCursor;
      cursorBlinkTime = 0.0f;
    }

    // Handle text input
    int key = GetCharPressed();
    while (key > 0)
    {
      if ((key >= 32) && (key <= 125) && (text.length() < maxLength))
      {
        text += (char)key;
      }
      key = GetCharPressed();
    }

    // Handle backspace
    if (IsKeyPressed(KEY_BACKSPACE) && text.length() > 0)
    {
      text.pop_back();
    }

    // Handle enter - for multi-line boxes add newline, for single-line deactivate
    if (IsKeyPressed(KEY_ENTER))
    {
      if (bounds.height > 60 && text.length() < maxLength)
      {
        text += '\n';
      }
      else
      {
        isActive = false;
      }
    }
  }
}

void TextBox::Draw()
{
  // Transparent background (slightly more visible when active)
  Color bgColor = isActive ? Color{255, 255, 255, 30} : Color{255, 255, 255, 15};
  DrawRectangleRec(bounds, bgColor);

  // White border (thicker when active)
  float borderThickness = isActive ? 4.0f : 3.0f;
  Color borderColor = isActive ? Color{255, 255, 255, 255} : Color{255, 255, 255, 180};
  DrawRectangleLinesEx(bounds, borderThickness, borderColor);

  // Text or placeholder
  std::string displayText = text;
  if (isPassword && !text.empty())
  {
    displayText = std::string(text.length(), '*');
  }

  if (displayText.empty() && !isActive)
  {
    DrawTextSpaced(placeholder.c_str(), bounds.x + 10, bounds.y + 10,
                   fontSize, Color{200, 200, 210, 255});
  }
  else
  {
    // Enable scissor mode to clip text within bounds
    BeginScissorMode(bounds.x, bounds.y, bounds.width, bounds.height);

    // For multi-line text boxes (height > 60), wrap text
    if (bounds.height > 60)
    {
      float currentY = bounds.y + 10;
      float maxWidth = bounds.width - 20;
      std::string currentLine = "";

      for (size_t i = 0; i < displayText.length(); i++)
      {
        char c = displayText[i];

        if (c == '\n')
        {
          DrawTextSpaced(currentLine.c_str(), bounds.x + 10, currentY, fontSize, {255, 255, 255, 255});
          currentY += fontSize + 5;
          currentLine = "";
        }
        else
        {
          std::string testLine = currentLine + c;
          int lineWidth = MeasureText(testLine.c_str(), fontSize);

          if (lineWidth > maxWidth)
          {
            DrawTextSpaced(currentLine.c_str(), bounds.x + 10, currentY, fontSize, {255, 255, 255, 255});
            currentY += fontSize + 5;
            currentLine = "";
            currentLine += c;
          }
          else
          {
            currentLine += c;
          }
        }
      }

      // Draw remaining text
      if (!currentLine.empty())
      {
        DrawTextSpaced(currentLine.c_str(), bounds.x + 10, currentY, fontSize, {255, 255, 255, 255});
      }

      // Cursor for multi-line
      if (isActive && showCursor)
      {
        int textWidth = MeasureText(currentLine.c_str(), fontSize);
        int cursorX = bounds.x + 10 + textWidth + 2;
        DrawRectangle(cursorX, currentY, 2, fontSize, {255, 255, 255, 255});
      }
    }
    else
    {
      // Single line text
      DrawTextSpaced(displayText.c_str(), bounds.x + 10, bounds.y + (bounds.height - fontSize) / 2,
                     fontSize, {255, 255, 255, 255});

      // Cursor
      if (isActive && showCursor)
      {
        int textWidth = MeasureText(displayText.c_str(), fontSize);
        int cursorX = bounds.x + 10 + textWidth + 2;
        int cursorY = bounds.y + 8;
        DrawRectangle(cursorX, cursorY, 2, fontSize, {255, 255, 255, 255});
      }
    }

    EndScissorMode();
  }
}

void TextBox::SetActive(bool active)
{
  isActive = active;
}

void TextBox::SetText(const char *newText)
{
  text = newText;
}

void TextBox::Clear()
{
  text = "";
}

void TextBox::SetPassword(bool password)
{
  isPassword = password;
}

void TextBox::SetPosition(float x, float y)
{
  bounds.x = x;
  bounds.y = y;
}

void TextBox::SetSize(float width, float height)
{
  bounds.width = width;
  bounds.height = height;
}

// Panel implementation
Panel::Panel()
{
  bounds = {0, 0, 200, 200};
  backgroundColor = UIColors::PANEL_BG;
  borderColor = UIColors::BORDER;
  borderRadius = 0.02f;
  hasShadow = true;
}

Panel::Panel(Rectangle bounds, Color bgColor)
{
  this->bounds = bounds;
  this->backgroundColor = bgColor;
  this->borderColor = UIColors::BORDER;
  this->borderRadius = 0.02f;
  this->hasShadow = true;
}

void Panel::Draw()
{
  // Fully transparent or very subtle background
  Color bgColor = {backgroundColor.r, backgroundColor.g, backgroundColor.b, 20};
  DrawRectangle(bounds.x, bounds.y, bounds.width, bounds.height, bgColor);

  // White border (4px thick like buttons)
  DrawRectangleLinesEx(bounds, 4, {255, 255, 255, 200});

  // Subtle inner white shading with margin (2px inset from border)
  Rectangle innerBounds = {bounds.x + 6, bounds.y + 6, bounds.width - 12, bounds.height - 12};
  Color innerShade = {255, 255, 255, 15};
  DrawRectangle(innerBounds.x, innerBounds.y, innerBounds.width, innerBounds.height, innerShade);
}
void Panel::SetPosition(float x, float y)
{
  bounds.x = x;
  bounds.y = y;
}

void Panel::SetSize(float width, float height)
{
  bounds.width = width;
  bounds.height = height;
}

// Label implementation
Label::Label()
{
  position = {0, 0};
  text = "Label";
  fontSize = 20;
  color = UIColors::TEXT_DARK;
  bold = false;
}

Label::Label(Vector2 position, const char *text, int fontSize, Color color)
{
  this->position = position;
  this->text = text;
  this->fontSize = fontSize;
  this->color = color;
  this->bold = false;
}

void Label::Draw()
{
  DrawTextSpaced(text.c_str(), position.x, position.y, fontSize, color);
}

void Label::SetText(const char *text)
{
  this->text = text;
}

void Label::SetPosition(float x, float y)
{
  position.x = x;
  position.y = y;
}

void Label::SetColor(Color color)
{
  this->color = color;
}

void Label::SetFontSize(int size)
{
  fontSize = size;
}

// EmailListItem implementation
EmailListItem::EmailListItem()
{
  bounds = {0, 0, 600, 80};
  sender = "Unknown";
  subject = "No Subject";
  preview = "";
  time = "";
  isRead = false;
  isImportant = false;
  isHovered = false;
  priority = 0;
}

EmailListItem::EmailListItem(Rectangle bounds, const char *sender, const char *subject,
                             const char *preview, const char *time, bool isRead,
                             bool isImportant, int priority)
{
  this->bounds = bounds;
  this->sender = sender;
  this->subject = subject;
  this->preview = preview;
  this->time = time;
  this->isRead = isRead;
  this->isImportant = isImportant;
  this->isHovered = false;
  this->priority = priority;
}

void EmailListItem::Update()
{
  Vector2 mousePos = GetMousePosition();
  isHovered = CheckCollisionPointRec(mousePos, bounds);
}

void EmailListItem::Draw()
{
  // Semi-transparent background for read emails, solid for unread
  Color bgColor;
  if (isRead)
  {
    bgColor = isHovered ? Color{245, 247, 250, 180} : Color{255, 255, 255, 180};
  }
  else
  {
    bgColor = isHovered ? Color{245, 247, 250, 255} : UIColors::UI_WHITE;
  }

  DrawRectangleRec(bounds, bgColor);
  DrawRectangleLinesEx(bounds, 1, UIColors::BORDER);

  // Priority indicator
  if (priority > 0)
  {
    Color priorityColor = UIColors::WARNING;
    if (priority >= 4)
      priorityColor = UIColors::DANGER;
    DrawRectangle(bounds.x, bounds.y, 5, bounds.height, priorityColor);
  }

  // Important star
  if (isImportant)
  {
    DrawTextSpaced("★", bounds.x + 15, bounds.y + 10, 24, UIColors::WARNING);
  }

  // Unread indicator - reserve space for it
  float unreadDotSpace = isRead ? 0 : 35;
  if (!isRead)
  {
    DrawCircle(bounds.x + bounds.width - 20, bounds.y + bounds.height / 2, 6, UIColors::PRIMARY);
  }

  float textX = bounds.x + (isImportant ? 50 : 20);

  // Calculate time width and reserve space for it
  int timeWidth = MeasureText(time.c_str(), 16);
  float timeX = bounds.x + bounds.width - timeWidth - unreadDotSpace - 15;

  // Calculate maximum width for sender text (leave space for time and dot)
  float maxTextWidth = timeX - textX - 10;

  // Sender (bold if unread) - dark text on white background
  int senderSize = isRead ? 20 : 22;
  std::string truncatedSender = sender;
  int senderWidth = MeasureText(sender.c_str(), senderSize);
  if (senderWidth > maxTextWidth)
  {
    // Truncate sender with ellipsis
    while (senderWidth > maxTextWidth - 20 && truncatedSender.length() > 3)
    {
      truncatedSender = truncatedSender.substr(0, truncatedSender.length() - 1);
      senderWidth = MeasureText((truncatedSender + "...").c_str(), senderSize);
    }
    truncatedSender += "...";
  }
  DrawTextSpaced(truncatedSender.c_str(), textX, bounds.y + 10, senderSize,
                 isRead ? Color{60, 60, 70, 200} : Color{40, 40, 50, 255});

  // Subject - dark text, truncate if needed
  std::string truncatedSubject = subject;
  int subjectWidth = MeasureText(subject.c_str(), 18);
  if (subjectWidth > maxTextWidth)
  {
    while (subjectWidth > maxTextWidth - 20 && truncatedSubject.length() > 3)
    {
      truncatedSubject = truncatedSubject.substr(0, truncatedSubject.length() - 1);
      subjectWidth = MeasureText((truncatedSubject + "...").c_str(), 18);
    }
    truncatedSubject += "...";
  }
  DrawTextSpaced(truncatedSubject.c_str(), textX, bounds.y + 35, 18,
                 isRead ? Color{60, 60, 70, 200} : Color{60, 60, 70, 255});

  // Preview - gray text on white background, truncate if needed
  Color previewColor = isRead ? Color{100, 110, 120, 200} : Color{100, 110, 120, 255};
  std::string truncatedPreview = preview;
  int previewWidth = MeasureText(preview.c_str(), 16);
  if (previewWidth > maxTextWidth)
  {
    while (previewWidth > maxTextWidth - 20 && truncatedPreview.length() > 3)
    {
      truncatedPreview = truncatedPreview.substr(0, truncatedPreview.length() - 1);
      previewWidth = MeasureText((truncatedPreview + "...").c_str(), 16);
    }
    truncatedPreview += "...";
  }
  DrawTextSpaced(truncatedPreview.c_str(), textX, bounds.y + 57, 16, previewColor);

  // Time - always draw at fixed position
  DrawTextSpaced(time.c_str(), timeX, bounds.y + 10, 16, previewColor);
}

bool EmailListItem::IsClicked()
{
  return isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void EmailListItem::SetPosition(float x, float y)
{
  bounds.x = x;
  bounds.y = y;
}

// ScrollableList implementation
ScrollableList::ScrollableList()
{
  bounds = {0, 0, 600, 400};
  scrollOffset = 0.0f;
  maxScroll = 0.0f;
  itemHeight = 80.0f;
  visibleItems = 5;
  isDragging = false;
}

ScrollableList::ScrollableList(Rectangle bounds, float itemHeight)
{
  this->bounds = bounds;
  this->scrollOffset = 0.0f;
  this->maxScroll = 0.0f;
  this->itemHeight = itemHeight;
  this->visibleItems = (int)(bounds.height / itemHeight);
  this->isDragging = false;
}

void ScrollableList::Update(int totalItems)
{
  maxScroll = std::max(0.0f, (totalItems * itemHeight) - bounds.height);

  Vector2 mousePos = GetMousePosition();
  if (CheckCollisionPointRec(mousePos, bounds))
  {
    float wheel = GetMouseWheelMove();
    scrollOffset -= wheel * 40.0f;
  }

  scrollOffset = std::max(0.0f, std::min(scrollOffset, maxScroll));
}

void ScrollableList::BeginScissorMode()
{
  ::BeginScissorMode((int)bounds.x, (int)bounds.y, (int)bounds.width, (int)bounds.height);
}

void ScrollableList::EndScissorMode()
{
  ::EndScissorMode();
}

int ScrollableList::GetFirstVisibleItem()
{
  return (int)(scrollOffset / itemHeight);
}

int ScrollableList::GetLastVisibleItem()
{
  return std::min((int)((scrollOffset + bounds.height) / itemHeight) + 1,
                  (int)((scrollOffset + bounds.height + itemHeight) / itemHeight));
}

// Sidebar implementation
Sidebar::Sidebar()
{
  bounds = {0, 0, 250, 800};
  backgroundColor = UIColors::SECONDARY;
  selectedIndex = 0;
}

Sidebar::Sidebar(Rectangle bounds)
{
  this->bounds = bounds;
  this->backgroundColor = UIColors::SECONDARY;
  this->selectedIndex = 0;
}

void Sidebar::Draw(const char **items, int itemCount, int unreadCounts[])
{
  // Semi-transparent white background
  DrawRectangleRec(bounds, {255, 255, 255, 60});
  DrawRectangleLinesEx(bounds, 3, {255, 255, 255, 200});

  float itemHeight = 60.0f;
  float startY = bounds.y + 20;

  for (int i = 0; i < itemCount; i++)
  {
    float y = startY + (i * itemHeight);
    Rectangle itemBounds = {bounds.x + 10, y, bounds.width - 20, itemHeight - 5};

    Color itemColor = (i == selectedIndex) ? Color{255, 255, 255, 120} : Color{255, 255, 255, 30};

    DrawRectangleRec(itemBounds, itemColor);
    DrawRectangleLinesEx(itemBounds, 2, {255, 255, 255, (unsigned char)(i == selectedIndex ? 255 : 150)});

    Vector2 mousePos = GetMousePosition();
    if (CheckCollisionPointRec(mousePos, itemBounds) && i != selectedIndex)
    {
      DrawRectangleRec(itemBounds, Color{255, 255, 255, 50});
    }

    // Text is dark on selected (white background), white on unselected
    Color textColor = (i == selectedIndex) ? Color{40, 40, 50, 255} : Color{255, 255, 255, 255};
    DrawTextSpaced(items[i], itemBounds.x + 10, y + 18, 20, textColor);

    // Draw unread count badge
    if (unreadCounts != nullptr && unreadCounts[i] > 0)
    {
      char countStr[10];
      sprintf(countStr, "%d", unreadCounts[i]);
      int countWidth = MeasureText(countStr, 16);
      float badgeX = bounds.x + bounds.width - countWidth - 25;
      float badgeY = y + 15;

      DrawCircle(badgeX + countWidth / 2 + 5, badgeY + 10, 12, UIColors::DANGER);
      DrawTextSpaced(countStr, badgeX, badgeY, 16, UIColors::UI_WHITE);
    }
  }
}

int Sidebar::Update(const char **items, int itemCount)
{
  Vector2 mousePos = GetMousePosition();
  float itemHeight = 60.0f;
  float startY = bounds.y + 20;

  int clickedIndex = -1;

  for (int i = 0; i < itemCount; i++)
  {
    float y = startY + (i * itemHeight);
    Rectangle itemBounds = {bounds.x, y, bounds.width, itemHeight - 5};

    if (CheckCollisionPointRec(mousePos, itemBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
      selectedIndex = i;
      clickedIndex = i;
    }
  }

  return clickedIndex;
}

// Modal implementation
Modal::Modal()
{
  bounds = {0, 0, 400, 200};
  title = "Message";
  message = "";
  isVisible = false;
}

Modal::Modal(const char *title, const char *message)
{
  this->bounds = {0, 0, 400, 200};
  this->title = title;
  this->message = message;
  this->isVisible = false;
}

void Modal::Show()
{
  isVisible = true;
}

void Modal::Hide()
{
  isVisible = false;
}

void Modal::Draw()
{
  if (!isVisible)
    return;

  // Overlay
  DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{0, 0, 0, 150});

  // Center modal
  bounds.x = (GetScreenWidth() - bounds.width) / 2;
  bounds.y = (GetScreenHeight() - bounds.height) / 2;

  // Modal background
  DrawRectangleRounded(bounds, 0.1f, 10, UIColors::UI_WHITE);
  DrawRectangleRoundedLines(bounds, 0.1f, 10, 2, UIColors::PRIMARY);

  // Title
  DrawTextSpaced(title.c_str(), bounds.x + 20, bounds.y + 20, 24, UIColors::DARK);

  // Message
  DrawTextSpaced(message.c_str(), bounds.x + 20, bounds.y + 60, 18, UIColors::TEXT_DARK);
}

void Modal::Update()
{
  if (isVisible && IsKeyPressed(KEY_ESCAPE))
  {
    Hide();
  }
}

void Modal::SetMessage(const char *msg)
{
  message = msg;
}

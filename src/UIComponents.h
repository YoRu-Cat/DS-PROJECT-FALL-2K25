#ifndef UICOMPONENTS_H
#define UICOMPONENTS_H

#include "raylib.h"
#include <string>
#include <functional>

// Color palette for a modern, beautiful UI (inspired by cyberpunk/game aesthetics)
namespace UIColors
{
  const Color PRIMARY = {100, 149, 237, 255};       // Cornflower Blue
  const Color PRIMARY_HOVER = {135, 169, 247, 255}; // Lighter Blue
  const Color SECONDARY = {75, 85, 110, 255};       // Dark Blue-Gray
  const Color SUCCESS = {72, 209, 204, 255};        // Turquoise
  const Color DANGER = {255, 99, 132, 255};         // Pink-Red
  const Color WARNING = {255, 206, 86, 255};        // Golden Yellow
  const Color INFO = {153, 102, 255, 255};          // Purple
  const Color LIGHT = {240, 244, 248, 255};         // Very Light Gray
  const Color DARK = {30, 39, 58, 255};             // Dark Navy
  const Color UI_WHITE = {255, 255, 255, 255};
  const Color TEXT_DARK = {30, 39, 58, 255};
  const Color TEXT_LIGHT = {240, 244, 248, 255};
  const Color BACKGROUND = {20, 30, 48, 255}; // Dark Blue Background
  const Color PANEL_BG = {45, 55, 75, 255};   // Panel Background
  const Color INPUT_BG = {255, 255, 255, 255};
  const Color BORDER = {135, 169, 247, 180}; // Bright Blue Border
  const Color SHADOW = {0, 0, 0, 30};
}

// Global custom font
extern Font *globalCustomFont;
void SetGlobalFont(Font *font);

// Helper functions
void DrawTextSpaced(const char *text, int posX, int posY, int fontSize, Color color, float spacing = 1.5f);
void DrawTextShadow(const char *text, int posX, int posY, int fontSize, Color color);
void DrawRoundedRectangleShadow(Rectangle rec, float roundness, int segments, Color color);

// Button component
class Button
{
private:
  Rectangle bounds;
  std::string text;
  Color color;
  Color hoverColor;
  Color textColor;
  bool isHovered;
  bool isPressed;
  int fontSize;
  float borderRadius;

public:
  Button();
  Button(Rectangle bounds, const char *text, Color color = UIColors::PRIMARY);

  void Update();
  void Draw();
  bool IsClicked();
  void SetText(const char *text);
  void SetPosition(float x, float y);
  void SetSize(float width, float height);
  void SetColor(Color color);
  Rectangle GetBounds() { return bounds; }
};

// TextBox component
class TextBox
{
private:
  Rectangle bounds;
  std::string text;
  std::string placeholder;
  bool isActive;
  bool isPassword;
  int maxLength;
  int fontSize;
  float cursorBlinkTime;
  bool showCursor;

public:
  TextBox();
  TextBox(Rectangle bounds, const char *placeholder = "", int maxLength = 100);

  void Update();
  void Draw();
  void SetActive(bool active);
  bool IsActive() { return isActive; }
  std::string GetText() { return text; }
  void SetText(const char *newText);
  void Clear();
  void SetPassword(bool password);
  void SetPosition(float x, float y);
  void SetSize(float width, float height);
  Rectangle GetBounds() { return bounds; }
};

// Panel component for grouping elements
class Panel
{
private:
  Rectangle bounds;
  Color backgroundColor;
  Color borderColor;
  float borderRadius;
  bool hasShadow;

public:
  Panel();
  Panel(Rectangle bounds, Color bgColor = UIColors::PANEL_BG);

  void Draw();
  void SetPosition(float x, float y);
  void SetSize(float width, float height);
  Rectangle GetBounds() { return bounds; }
};

// Label component
class Label
{
private:
  Vector2 position;
  std::string text;
  int fontSize;
  Color color;
  bool bold;

public:
  Label();
  Label(Vector2 position, const char *text, int fontSize = 20, Color color = UIColors::TEXT_DARK);

  void Draw();
  void SetText(const char *text);
  void SetPosition(float x, float y);
  void SetColor(Color color);
  void SetFontSize(int size);
};

// Email list item component
class EmailListItem
{
private:
  Rectangle bounds;
  std::string sender;
  std::string subject;
  std::string preview;
  std::string time;
  bool isRead;
  bool isImportant;
  bool isHovered;
  int priority;

public:
  EmailListItem();
  EmailListItem(Rectangle bounds, const char *sender, const char *subject,
                const char *preview, const char *time, bool isRead = false,
                bool isImportant = false, int priority = 0);

  void Update();
  void Draw();
  bool IsClicked();
  void SetPosition(float x, float y);
  Rectangle GetBounds() { return bounds; }
};

// Scrollable list container
class ScrollableList
{
private:
  Rectangle bounds;
  float scrollOffset;
  float maxScroll;
  float itemHeight;
  int visibleItems;
  bool isDragging;

public:
  ScrollableList();
  ScrollableList(Rectangle bounds, float itemHeight);

  void Update(int totalItems);
  void BeginScissorMode();
  void EndScissorMode();
  float GetScrollOffset() { return scrollOffset; }
  int GetFirstVisibleItem();
  int GetLastVisibleItem();
  void SetPosition(float x, float y)
  {
    bounds.x = x;
    bounds.y = y;
  }
  void SetSize(float width, float height)
  {
    bounds.width = width;
    bounds.height = height;
    visibleItems = (int)(height / itemHeight);
  }
  Rectangle GetBounds() { return bounds; }
};

// Sidebar menu component
class Sidebar
{
private:
  Rectangle bounds;
  Color backgroundColor;
  int selectedIndex;

public:
  Sidebar();
  Sidebar(Rectangle bounds);

  void Draw(const char **items, int itemCount, int unreadCounts[] = nullptr);
  int Update(const char **items, int itemCount);
  int GetSelectedIndex() { return selectedIndex; }
  void SetSelectedIndex(int index) { selectedIndex = index; }
  Rectangle GetBounds() { return bounds; }
};

// Modal dialog component
class Modal
{
private:
  Rectangle bounds;
  std::string title;
  std::string message;
  bool isVisible;

public:
  Modal();
  Modal(const char *title, const char *message);

  void Show();
  void Hide();
  void Draw();
  void Update();
  bool IsVisible() { return isVisible; }
  void SetMessage(const char *msg);
};

// Helper function to draw text with shadow
void DrawTextShadow(const char *text, int posX, int posY, int fontSize, Color color);

// Helper function to draw rounded rectangle with shadow
void DrawRoundedRectangleShadow(Rectangle rec, float roundness, int segments, Color color);

#endif // UICOMPONENTS_H

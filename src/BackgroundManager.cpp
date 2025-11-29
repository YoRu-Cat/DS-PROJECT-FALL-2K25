#include "BackgroundManager.h"
#include <cmath>

BackgroundManager::BackgroundManager(int width, int height)
{
  screenWidth = width;
  screenHeight = height;
  textureLoaded = false;
  overlayColor = {0, 0, 0, 255};
  overlayAlpha = 0.3f;
}

BackgroundManager::~BackgroundManager()
{
  UnloadBackground();
}

bool BackgroundManager::LoadBackground(const char *imagePath)
{
  UnloadBackground();

  Image image = LoadImage(imagePath);
  if (image.data == nullptr)
  {
    return false;
  }

  backgroundTexture = LoadTextureFromImage(image);
  UnloadImage(image);
  textureLoaded = true;

  return true;
}

void BackgroundManager::SetOverlay(Color color, float alpha)
{
  overlayColor = color;
  overlayAlpha = alpha;
}

void BackgroundManager::DrawBackground()
{
  DrawFittedBackground();
}

void BackgroundManager::DrawFittedBackground()
{
  if (!textureLoaded)
  {
    DrawRectangle(0, 0, screenWidth, screenHeight, {20, 30, 48, 255});
    return;
  }

  // Calculate scaling to fit the screen while maintaining aspect ratio
  float scaleX = (float)screenWidth / (float)backgroundTexture.width;
  float scaleY = (float)screenHeight / (float)backgroundTexture.height;
  float scale = (scaleX > scaleY) ? scaleX : scaleY;

  int drawWidth = (int)(backgroundTexture.width * scale);
  int drawHeight = (int)(backgroundTexture.height * scale);
  int drawX = (screenWidth - drawWidth) / 2;
  int drawY = (screenHeight - drawHeight) / 2;

  // Draw the scaled background
  DrawTexturePro(
      backgroundTexture,
      {0, 0, (float)backgroundTexture.width, (float)backgroundTexture.height},
      {(float)drawX, (float)drawY, (float)drawWidth, (float)drawHeight},
      {0, 0},
      0.0f,
      WHITE);

  DrawRectangle(0, 0, screenWidth, screenHeight,
                {overlayColor.r, overlayColor.g, overlayColor.b,
                 (unsigned char)(overlayAlpha * 255)});
}

void BackgroundManager::DrawStretchedBackground()
{
  if (!textureLoaded)
  {
    DrawRectangle(0, 0, screenWidth, screenHeight, {20, 30, 48, 255});
    return;
  }

  DrawTexturePro(
      backgroundTexture,
      {0, 0, (float)backgroundTexture.width, (float)backgroundTexture.height},
      {0, 0, (float)screenWidth, (float)screenHeight},
      {0, 0},
      0.0f,
      WHITE);

  DrawRectangle(0, 0, screenWidth, screenHeight,
                {overlayColor.r, overlayColor.g, overlayColor.b,
                 (unsigned char)(overlayAlpha * 255)});
}

void BackgroundManager::DrawTiledBackground()
{
  if (!textureLoaded)
  {
    DrawRectangle(0, 0, screenWidth, screenHeight, {20, 30, 48, 255});
    return;
  }

  for (int y = 0; y < screenHeight; y += backgroundTexture.height)
  {
    for (int x = 0; x < screenWidth; x += backgroundTexture.width)
    {
      DrawTexture(backgroundTexture, x, y, WHITE);
    }
  }

  DrawRectangle(0, 0, screenWidth, screenHeight,
                {overlayColor.r, overlayColor.g, overlayColor.b,
                 (unsigned char)(overlayAlpha * 255)});
}

void BackgroundManager::UnloadBackground()
{
  if (textureLoaded)
  {
    UnloadTexture(backgroundTexture);
    textureLoaded = false;
  }
}

#ifndef BACKGROUNDMANAGER_H
#define BACKGROUNDMANAGER_H

#include "raylib.h"
#include <string>

class BackgroundManager
{
private:
  Texture2D backgroundTexture;
  bool textureLoaded;
  Color overlayColor;
  float overlayAlpha;
  int screenWidth;
  int screenHeight;

public:
  BackgroundManager(int width, int height);
  ~BackgroundManager();

  bool LoadBackground(const char *imagePath);
  void SetOverlay(Color color, float alpha);
  void DrawBackground();
  void DrawFittedBackground();
  void DrawStretchedBackground();
  void DrawTiledBackground();
  void UnloadBackground();
  bool IsLoaded() { return textureLoaded; }
};

#endif

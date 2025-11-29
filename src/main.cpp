#include "raylib.h"
#include "YoRuSplScr.h"
#include "EmailUI.h"

using namespace std;

int main()
{
	// Initialization
	int displayWidth = 1920;
	int displayHeight = 1080;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_FULLSCREEN_MODE | FLAG_VSYNC_HINT);
	InitWindow(displayWidth, displayHeight, "Email Management System");
	SetTargetFPS(60);

	// Load custom font
	Font customFont = LoadFont("Font/monogram.ttf");
	SetTextureFilter(customFont.texture, TEXTURE_FILTER_POINT);

	// Set the global font for UI components
	SetGlobalFont(&customFont);

	SplashScreen splash("resources/YoRu_n.gif", 10, 3.0f, 10.0f);

	// Initialize Email UI (will be shown after splash)
	EmailUI *emailUI = nullptr;
	bool splashComplete = false;

	// Main application loop
	while (!WindowShouldClose())
	{
		float deltaTime = GetFrameTime();

		// Update splash screen
		if (!splashComplete)
		{
			if (splash.Update(deltaTime))
			{
				splashComplete = true;
				// Initialize email UI after splash is complete
				emailUI = new EmailUI(displayWidth, displayHeight);
			}
		}

		// Update email UI if splash is complete
		if (splashComplete && emailUI != nullptr)
		{
			emailUI->Update(deltaTime);
		}

		// Drawing
		BeginDrawing();
		ClearBackground(BLACK);

		if (!splashComplete)
		{
			// Draw splash screen
			splash.Draw();
		}
		else if (emailUI != nullptr)
		{
			// Draw email UI
			emailUI->Draw();
		}

		EndDrawing();
	}

	// Cleanup
	if (emailUI != nullptr)
	{
		delete emailUI;
	}

	UnloadFont(customFont);
	CloseWindow();
	return 0;
}

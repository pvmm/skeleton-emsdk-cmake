#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main()
{
	InitWindow(800, 800, "raygui - controls test suite");
	SetTargetFPS(60);

	bool fileLoaded = false;
	bool showAboutBox = false;

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

		if (GuiButton((Rectangle){ 24, 24, 120, 30 }, "#191#Button"))
			fileLoaded = true;
		if (GuiButton((Rectangle){ 24, 70, 120, 30 }, "#191#About"))
			showAboutBox = true;

		if (showAboutBox)
		{
			int result = GuiMessageBox((Rectangle){ 275, 350, 250, 100 },
			        "#191#About", "Hi! This is a message!", "OK");

			if (result >= 0) showAboutBox = false;
		}

		EndDrawing();
	}

	CloseWindow();
	return 0;
}

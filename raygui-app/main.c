#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#if defined(PLATFORM_WEB)
	#define CUSTOM_MODAL_DIALOGS            // Force custom modal dialogs usage
	#include <emscripten/emscripten.h>      // Emscripten library - LLVM to JavaScript compiler
#endif

#undef RAYGUI_IMPLEMENTATION                // Avoid including raygui implementation again

#define GUI_FILE_DIALOGS_IMPLEMENTATION
#include "gui_file_dialogs.h"               // GUI: File Dialogs

#include "functions.h"

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static const char *tool_name = TOOL_NAME;
static const char *tool_version = TOOL_VERSION;
static const char *tool_description = TOOL_DESCRIPTION;

// NOTE: Max length depends on OS, in Windows MAX_PATH = 256
static char in_filename[512] = { 0 };
static char out_filename[512] = { 0 };
bool save_changes_required = false;

int main()
{
	(void)tool_description;
	(void)out_filename;

	InitWindow(800, 800, "raygui - controls test suite");
	SetTargetFPS(60);

	bool show_load_file_dialog = false;
	bool show_about_box = false;

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

		if (show_load_file_dialog) {
#if defined(CUSTOM_MODAL_DIALOGS) 
			int result = GuiFileDialog(DIALOG_MESSAGE, "Load file", in_filename, "OK", "Just drag and drop your file!");
#else
			int result = GuiFileDialog(DIALOG_OPEN_FILE, "Load file", in_filename, "*.*", "All files (*.*)");
#endif
			if (result == 1)
			{
				load_file(in_filename);
				SetWindowTitle(TextFormat("%s v%s | File: %s", tool_name, tool_version, GetFileName(in_filename)));
				save_changes_required = false;
			}

			if (result >= 0) show_load_file_dialog = false;
		}
		if (GuiButton((Rectangle){ 24, 24, 120, 30 }, "#11#Open file...")) {
			show_load_file_dialog = true;
		}
		if (GuiButton((Rectangle){ 24, 70, 120, 30 }, "#191#About"))
			show_about_box = true;

		if (show_about_box)
		{
			int result = GuiMessageBox((Rectangle){ 275, 350, 250, 100 },
			        "#191#About", "Hi! This is a message!", "OK");

			if (result >= 0) show_about_box = false;
		}

		EndDrawing();
	}

	CloseWindow();
	return 0;
}

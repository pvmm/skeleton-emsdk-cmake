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

#define ALLOWED_FILE_EXT ".txt"

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static const char* tool_name = TOOL_NAME;
static const char* tool_version = TOOL_VERSION;
static const char* tool_description = TOOL_DESCRIPTION;

static char* load_error_message = NULL;

// NOTE: Max length depends on OS, in Windows MAX_PATH = 256
static char in_filename[512] = { 0 };
static char out_filename[512] = { 0 };
bool save_changes_required = false;

int main()
{
	(void)tool_description;
	(void)out_filename;
	(void)tool_name;
	(void)tool_version;

	InitWindow(800, 600, "raygui - controls test suite");
	SetTargetFPS(60);

	bool show_load_file_dialog = false;
	bool show_about_box_dialog = false;

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

		if (IsFileDropped())
		{
			load_error_message = "Unexpected file action";
			UnloadDroppedFiles(LoadDroppedFiles());
		}
		if (load_error_message != NULL)
		{
			if (show_message("Error", load_error_message) != -1) {
				load_error_message = NULL;
			}
		}
		if (GuiButton((Rectangle){ 24, 24, 120, 30 }, "#11#Open file...") && load_error_message == NULL)
		{
			show_load_file_dialog = true;
		}
		if (show_load_file_dialog)
		{
#if defined(CUSTOM_MODAL_DIALOGS) 
			int result = GuiFileDialog(DIALOG_MESSAGE, "Load file", in_filename, "OK", "Just drag and drop your file!");
			if (IsFileDropped())
			{
				FilePathList dropped_files = LoadDroppedFiles();
				if (IsFileExtension(dropped_files.paths[0], ALLOWED_FILE_EXT))
				{
					load_error_message = NULL;
					for (int i = 0; i < dropped_files.count; ++i) {
						load_file(dropped_files.paths[i]);
					}
				}
				else
				{
					load_error_message = "Wrong file type";
				}
				UnloadDroppedFiles(dropped_files);
				show_load_file_dialog = false;
			}
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
		if (GuiButton((Rectangle){ 24, 70, 120, 30 }, "#191#About"))
			show_about_box_dialog = true;

		if (show_about_box_dialog)
		{
			if (show_about_box() >= 0) show_about_box_dialog = false;
		}

		EndDrawing();
	}

	CloseWindow();
	return 0;
}

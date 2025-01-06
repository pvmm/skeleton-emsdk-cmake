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

#include <stdlib.h>

#define ALLOWED_FILE_EXT    ".txt"
#define MAX_ERRORS          5

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static const char* tool_name = TOOL_NAME;
static const char* tool_version = TOOL_VERSION;
static const char* tool_description = TOOL_DESCRIPTION;

static char *error_messages[MAX_ERRORS] = { NULL };
static int error_index = -1;

// NOTE: Max length depends on OS, in Windows MAX_PATH = 256
static char in_filename[512] = { 0 };
static char out_filename[512] = { 0 };
bool save_changes_required = false;

void append_error_message(char* fmt, ...)
{
	if (error_index < MAX_ERRORS)
	{
		va_list ap;
		va_start(ap, fmt);
		error_messages[++error_index] = malloc(256);
		vsnprintf(error_messages[error_index], 256, fmt, ap);
		va_end(ap);
	}
}

void drop_error_message()
{
	if (error_index > -1)
	{
		free(error_messages[error_index--]);
	}
}

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

		if (!show_load_file_dialog && IsFileDropped())
		{
			append_error_message("%s", "Unexpected file dragging. Click on the \"Open File...\" button first.");
			UnloadDroppedFiles(LoadDroppedFiles());
		}
		if (error_index >= 0)
		{
			if (show_message("Error", error_messages[error_index]) != -1)
			{
				drop_error_message();
			}
		}
		if (GuiButton((Rectangle){ 24, 24, 120, 30 }, "#11#Open file...") && error_index == -1)
		{
			show_load_file_dialog = true;
		}
		if (show_load_file_dialog)
		{
#if defined(CUSTOM_MODAL_DIALOGS) 
			int result = GuiFileDialog(DIALOG_MESSAGE, "Load file", in_filename, "OK",
							"Just drag and drop your file!");
			if (result == -1 && IsFileDropped())
			{
				FilePathList dropped_files = LoadDroppedFiles();
				for (int i = 0; i < dropped_files.count; ++i)
				{
					if (IsFileExtension(dropped_files.paths[i], ALLOWED_FILE_EXT))
					{
						load_file(dropped_files.paths[i]);
					}
					else
					{
						append_error_message("Wrong file type: %s", get_file_name(dropped_files.paths[i]));
					}
				}
				UnloadDroppedFiles(dropped_files);
				show_load_file_dialog = false;
			}
#else
			int result = GuiFileDialog(DIALOG_OPEN_FILE, "Load file", in_filename, "*.*", "All files (*.*)");
//#endif
			if (result == 1)
			{
				load_file(in_filename);
				SetWindowTitle(TextFormat("%s v%s | File: %s", tool_name, tool_version, GetFileName(in_filename)));
				save_changes_required = false;
			}
#endif
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

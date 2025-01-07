#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#if defined(PLATFORM_WEB)
	#define CUSTOM_MODAL_DIALOGS            // Force custom modal dialogs usage
	#include <emscripten/emscripten.h>      // Emscripten library - LLVM to JavaScript compiler
#endif

#undef RAYGUI_IMPLEMENTATION                // Avoid including raygui implementation again

#include "functions.h"

#define ALLOWED_FILE_EXT    ".txt"

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static const char* tool_name = TOOL_NAME;
static const char* tool_version = TOOL_VERSION;
static const char* tool_description = TOOL_DESCRIPTION;

// NOTE: Max length depends on OS, in Windows MAX_PATH = 256
static char in_filename[512] = { 0 };
static char out_filename[512] = { 0 };
//bool save_changes_required = false;

int main()
{
	(void)tool_description;
	(void)out_filename;
	(void)tool_name;
	(void)tool_version;

	InitWindow(800, 600, "Raygui Sample App");
	SetTargetFPS(60);

	bool request_load_dialog = false;
	bool request_about_box = false;

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

		process_errors();

		//disable_gui(has_error() || request_load_dialog || show_about_box_dialog);
		if (show_button((Rectangle){ 24, 24, 120, 30 }, "#11#Open file..."))
		{
			printf("bla!\n");
			request_load_dialog = true;
		}

		//disable_gui(has_error() || show_about_box_dialog);
		int result;
		if (request_load_dialog && (result = show_load_dialog("Load file", in_filename, "*.txt")))
		{
#if defined(CUSTOM_MODAL_DIALOGS) 
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
				unload_dropped_files();
				reset_gui_lock(P_FILE_DIALOG);
			}
#else
			if (result == 1)
			{
				load_file(in_filename);
				SetWindowTitle(TextFormat("%s v%s | File: %s", tool_name, tool_version, GetFileName(in_filename)));
				save_changes_required = false;
			}
#endif
			if (result >= 0) request_load_dialog = false;
		}

		//disable_gui(has_error() || request_load_dialog || show_about_box_dialog);
		if (show_button((Rectangle){ 24, 70, 120, 30 }, "#191#About..."))
			request_about_box = true;

		//disable_gui(has_error() || request_load_dialog);
		if (request_about_box && show_about_box() >= 0)
		{
			request_about_box = false;
		}

		//disable_gui(false);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}

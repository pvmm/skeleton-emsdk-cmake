#include "raygui.h"
#include <stdio.h>

void load_file(char* filename)
{
	fprintf(stderr, "function load_file(%s) called\n", filename);
}

int show_about_box()
{
	fprintf(stderr, "show_about_box() called.");
	return GuiMessageBox((Rectangle){ GetScreenWidth() / 2 - 160, GetScreenHeight() / 2 - 120, 320, 120 }, "#191#About", "Hi! This is a message", "OK");
}

int show_message(char* title, char* message)
{
	return GuiMessageBox((Rectangle){ GetScreenWidth() / 2 - 160, GetScreenHeight() / 2 - 120, 320, 120 }, title, message, "OK");
}

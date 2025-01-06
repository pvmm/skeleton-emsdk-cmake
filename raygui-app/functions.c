#include "raygui.h"

#include <stdio.h>
#include <string.h>

void load_file(char* filename)
{
	fprintf(stderr, "function load_file(%s) called\n", filename);
}

int show_about_box()
{
	fprintf(stderr, "show_about_box() called.");
	return GuiMessageBox((Rectangle){ GetScreenWidth() / 2 - 200, GetScreenHeight() / 2 - 180, 400, 180 }, "#191#About", "Hi! This is a message", "OK");
}

int show_message(char* title, char* message)
{
	return GuiMessageBox((Rectangle){ GetScreenWidth() / 2 - 200, GetScreenHeight() / 2 - 180, 400, 180 }, title, message, "OK");
}

char* get_file_name(char* path)
{
	char *s;

	if (*(s = strrchr(path, '/')) != '\0') {
		return s + 1;
	}

	return path;
}

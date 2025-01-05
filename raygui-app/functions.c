#include "raygui.h"
#include <stdio.h>

void load_file(char* filename)
{
	fprintf(stderr, "function load_file(%s) called\n", filename);
}

int show_about_box()
{
	fprintf(stderr, "show_about_box() called.");
	return GuiMessageBox((Rectangle){ 275, 350, 250, 100 }, "#191#About", "Hi! This is a message!", "OK");
}

int show_message(char* message)
{
	return GuiMessageBox((Rectangle){ 275, 350, 250, 100 }, "#191#Message", message, "OK");
}

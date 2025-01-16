#define CLAY_IMPLEMENTATION
#include "clay.h"
#ifdef PLATFORM_DESKTOP
#include "renderers/raylib/clay_renderer_raylib.c"
#endif

#define RAYLIB_VECTOR2_TO_CLAY_VECTOR2(vector) (Clay_Vector2) { .x = (vector).x, .y = (vector).y }

const int screenWidth = 1024;
const int screenHeight = 768;
const Clay_Color COLOR_LIGHT = (Clay_Color) {224, 215, 210, 255};
const Clay_Color COLOR_RED = (Clay_Color) {168, 66, 28, 255};
const Clay_Color COLOR_ORANGE = (Clay_Color) {225, 138, 50, 255};
const int FONT_ID_BODY_16 = 0;
Texture2D profilePicture;

Clay_LayoutConfig sidebarItemLayout = (Clay_LayoutConfig) {
	.sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(50) },
};

void SidebarItemComponent() {
	CLAY(CLAY_LAYOUT(sidebarItemLayout), CLAY_RECTANGLE({ .color = COLOR_ORANGE })) {}
}

Clay_RenderCommandArray CreateLayout() {
	Clay_SetLayoutDimensions((Clay_Dimensions) { screenWidth, screenHeight });
	Clay_BeginLayout();

	CLAY(
		CLAY_ID("OuterContainer"),
		CLAY_LAYOUT({
			.sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)},
			.padding = CLAY_PADDING_ALL(16),
			.childGap = 16 }),
		CLAY_RECTANGLE({ .color = {250,250,255,255} })
	) {
		CLAY(
			CLAY_ID("SideBar"),
			CLAY_LAYOUT({
					.layoutDirection = CLAY_TOP_TO_BOTTOM,
					.sizing = { .width = CLAY_SIZING_FIXED(300), .height = CLAY_SIZING_GROW(0) },
					.padding = CLAY_PADDING_ALL(16), .childGap = 16
			}),
			CLAY_RECTANGLE({ .color = COLOR_LIGHT })
		) {
			CLAY(
				CLAY_ID("ProfilePictureOuter"),
				CLAY_LAYOUT({
					.sizing = { .width = CLAY_SIZING_GROW(0) },
					.padding = CLAY_PADDING_ALL(16),
					.childGap = 16,
					.childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
				}),
				CLAY_RECTANGLE({ .color = COLOR_RED })
			) {
				CLAY(
					CLAY_ID("ProfilePicture"),
					CLAY_LAYOUT({
						.sizing = { .width = CLAY_SIZING_FIXED(60), .height = CLAY_SIZING_FIXED(60) }
					}),
					CLAY_IMAGE({ .imageData = &profilePicture, .sourceDimensions = {60, 60} }))
				{}
				CLAY_TEXT(CLAY_STRING("Clay - UI Library"),
					CLAY_TEXT_CONFIG({ .fontSize = 24, .textColor = {255, 255, 255, 255} })
				);
			}

			// Standard C code like loops etc work inside components
			for (int i = 0; i < 5; i++) {
				SidebarItemComponent();
			}
		}

		CLAY(
			CLAY_ID("MainContent"),
			CLAY_LAYOUT({ .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }}),
			CLAY_RECTANGLE({ .color = COLOR_LIGHT })
		) {}
	}

	return Clay_EndLayout();
}

void HandleClayErrors(Clay_ErrorData errorData) {
	printf("%s", errorData.errorText.chars);
}

int main(void)
{
	uint64_t totalMemorySize = Clay_MinMemorySize();
	Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, malloc(totalMemorySize));
	Clay_SetMeasureTextFunction(Raylib_MeasureText);
	Clay_Initialize(arena,
		(Clay_Dimensions) { screenWidth, screenHeight },
		(Clay_ErrorHandler) { HandleClayErrors });

	Clay_Raylib_Initialize(screenWidth, screenHeight, "Introducing Clay Demo",
		FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
	profilePicture = LoadTextureFromImage(LoadImage("resources/profile-picture.png"));

	// set font
	Raylib_fonts[FONT_ID_BODY_16] = (Raylib_Font) {
		.font = LoadFontEx("resources/Roboto-Regular.ttf", 48, 0, 400),
		.fontId = FONT_ID_BODY_16
	};
	SetTextureFilter(Raylib_fonts[FONT_ID_BODY_16].font.texture, TEXTURE_FILTER_BILINEAR);

	while (!WindowShouldClose()) {
		Clay_Vector2 mousePosition = RAYLIB_VECTOR2_TO_CLAY_VECTOR2(GetMousePosition());
		Clay_SetPointerState((Clay_Vector2) { mousePosition.x, mousePosition.y }, IsMouseButtonDown(0));
		Vector2 scrollDelta = GetMouseWheelMoveV();
		Clay_UpdateScrollContainers(true, (Clay_Vector2) { scrollDelta.x, scrollDelta.y }, GetFrameTime());
		Clay_RenderCommandArray renderCommands = CreateLayout();

		BeginDrawing();
		ClearBackground(BLACK);
		Clay_Raylib_Render(renderCommands);
		EndDrawing();
	}

	return 0;
}

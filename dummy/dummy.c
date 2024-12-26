#ifdef PLATFORM_DESKTOP
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#endif // PLATFORM_DESKTOP

#define CLAY_EXTEND_CONFIG_RECTANGLE Clay_String link; bool cursorPointer;
#define CLAY_EXTEND_CONFIG_IMAGE Clay_String sourceURL;
#define CLAY_EXTEND_CONFIG_TEXT bool disablePointerEvents;

#define CLAY_IMPLEMENTATION
#include "clay.h"
#ifdef PLATFORM_DESKTOP
#include "renderers/raylib/clay_renderer_raylib.c"
#endif // PLATFORM_DESKTOP

double windowWidth = 1024, windowHeight = 768;
float modelPageOneZRotation = 0;
uint32_t ACTIVE_RENDERER_INDEX = 0;

const uint32_t FONT_ID_BODY_16 = 0;
/*
const uint32_t FONT_ID_TITLE_56 = 1;
const uint32_t FONT_ID_BODY_24 = 2;
const uint32_t FONT_ID_BODY_36 = 3;
const uint32_t FONT_ID_TITLE_36 = 4;
const uint32_t FONT_ID_MONOSPACE_24 = 5;
*/

const Clay_Color COLOR_LIGHT = (Clay_Color) {244, 235, 230, 255};
const Clay_Color COLOR_LIGHT_HOVER = (Clay_Color) {224, 215, 210, 255};
const Clay_Color COLOR_RED = (Clay_Color) {168, 66, 28, 255};
const Clay_Color COLOR_RED_HOVER = (Clay_Color) {148, 46, 8, 255};
const Clay_Color COLOR_ORANGE = (Clay_Color) {225, 138, 50, 255};
const Clay_Color COLOR_BLUE = (Clay_Color) {111, 173, 162, 255};

// Colors for top stripe
const Clay_Color COLOR_TOP_BORDER_1 = (Clay_Color) {168, 66, 28, 255};
const Clay_Color COLOR_TOP_BORDER_2 = (Clay_Color) {223, 110, 44, 255};
const Clay_Color COLOR_TOP_BORDER_3 = (Clay_Color) {225, 138, 50, 255};
const Clay_Color COLOR_TOP_BORDER_4 = (Clay_Color) {236, 189, 80, 255};
const Clay_Color COLOR_TOP_BORDER_5 = (Clay_Color) {240, 213, 137, 255};

const Clay_Color COLOR_BLOB_BORDER_1 = (Clay_Color) {168, 66, 28, 255};
const Clay_Color COLOR_BLOB_BORDER_2 = (Clay_Color) {203, 100, 44, 255};
const Clay_Color COLOR_BLOB_BORDER_3 = (Clay_Color) {225, 138, 50, 255};
const Clay_Color COLOR_BLOB_BORDER_4 = (Clay_Color) {236, 159, 70, 255};
const Clay_Color COLOR_BLOB_BORDER_5 = (Clay_Color) {240, 189, 100, 255};

float animationLerpValue = -1.0f;

void RenderHeaderButton(Clay_String text)
{
    CLAY(
        CLAY_LAYOUT({ .padding = { 16, 8 } }),
        CLAY_RECTANGLE({
            .color = { 140, 140, 140, 255 },
            .cornerRadius = CLAY_CORNER_RADIUS(5),
        })
    ) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 16,
            .textColor = { 255, 255, 255, 255 },
        }));
    }
}

Clay_RenderCommandArray CreateLayout(bool mobileScreen, float lerpValue) {
    Clay_Sizing expanding = {
        .width = CLAY_SIZING_GROW(),
        .height = CLAY_SIZING_GROW(),
    };

    Clay_BeginLayout();
    CLAY(
        CLAY_ID("OuterContainer"),
        CLAY_RECTANGLE({ .color = { 43, 41, 51, 255 } }),
        CLAY_LAYOUT({
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = expanding,
            .padding = { 16, 16 },
        })
    ) {
        CLAY(
            CLAY_ID("HeaderBar"),
            CLAY_RECTANGLE({
                .color = { 90, 90, 90, 255 },
                .cornerRadius = CLAY_CORNER_RADIUS(8),
            }),
            CLAY_LAYOUT({
                .sizing = {
                    .width = CLAY_SIZING_GROW(),
                    .height = CLAY_SIZING_FIXED(90),
                },
                .padding = { .x = 16 },
                .childGap = 16,
                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
            })
        ) {
            RenderHeaderButton(CLAY_STRING("Open..."));
        }
        CLAY(
            CLAY_ID("LowerContent"),
            CLAY_LAYOUT({
                .sizing = expanding
            })
        ) {
        }
    }
    return Clay_EndLayout();
}

bool debugModeEnabled = false;

CLAY_WASM_EXPORT("UpdateDrawFrame") Clay_RenderCommandArray UpdateDrawFrame(float width, float height, float mouseWheelX, float mouseWheelY, float mousePositionX, float mousePositionY, bool isTouchDown, bool isMouseDown, bool arrowKeyDownPressedThisFrame, bool arrowKeyUpPressedThisFrame, bool dKeyPressedThisFrame, float deltaTime) {
    windowWidth = width;
    windowHeight = height;
    Clay_SetLayoutDimensions((Clay_Dimensions) { width, height });

    if (deltaTime == deltaTime) { // NaN propagation can cause pain here
        animationLerpValue += deltaTime;
        if (animationLerpValue > 1) {
            animationLerpValue -= 2;
        }
    }

    if (dKeyPressedThisFrame) {
        debugModeEnabled = !debugModeEnabled;
        Clay_SetDebugModeEnabled(debugModeEnabled);
    }
    //Clay_SetCullingEnabled(ACTIVE_RENDERER_INDEX == 1);
    //Clay_SetExternalScrollHandlingEnabled(ACTIVE_RENDERER_INDEX == 0);

    Clay__debugViewHighlightColor = (Clay_Color) {105,210,231, 120};

    Clay_SetPointerState((Clay_Vector2) {mousePositionX, mousePositionY}, isMouseDown || isTouchDown);

    bool isMobileScreen = windowWidth < 750;
    if (debugModeEnabled) {
        isMobileScreen = windowWidth < 950;
    }
    return CreateLayout(isMobileScreen, animationLerpValue < 0 ? (animationLerpValue + 1) : (1 - animationLerpValue));
    //----------------------------------------------------------------------------------
}

#ifdef PLATFORM_DESKTOP
bool IsTouchButtonDown()
{
    return IsMouseButtonDown(MOUSE_BUTTON_LEFT);
}
#endif // PLATFORM_DESKTOP

#ifdef PLATFORM_DESKTOP
Font load_font_check(const char* filename, int fontSize, int *codepoints, int codepointCount)
{
		Font f = LoadFontEx(filename, fontSize, codepoints, codepointCount);
		assert(f.baseSize != 0);
		return f;
}
#endif // PLATFORM_DESKTOP

int main() {
#ifdef PLATFORM_DESKTOP
#error 1
    printf("Desktop platform\n");
    uint64_t totalMemorySize = Clay_MinMemorySize();
    Clay_Arena clayMemory = (Clay_Arena) {
        .label = CLAY_STRING("Clay Memory Arena"),
        .memory = malloc(totalMemorySize),
        .capacity = totalMemorySize
    };
    Clay_Initialize(clayMemory, (Clay_Dimensions) {
        .width = windowWidth,
        .height = windowHeight,
    });
    Clay_SetMeasureTextFunction(Raylib_MeasureText);
    Clay_Raylib_Initialize(1024, 768, "VGM tools Online",
                           FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    printf("Initial dimensions: %i, %i\n", GetScreenWidth(), GetScreenHeight());

    Raylib_fonts[FONT_ID_BODY_16] = (Raylib_Font) {
        .font = load_font_check("resources/Roboto-Regular.ttf", 32, 0, 400),
        .fontId = FONT_ID_BODY_16,
    };
    SetTextureFilter(Raylib_fonts[FONT_ID_BODY_16].font.texture, TEXTURE_FILTER_BILINEAR);

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // FIXME: GetScreenWidth() and GetScreenHeight() are reporting wrong dimensions
        // when FLAG_WINDOW_HIGHDPI is used
        //printf("dimensions: %i, %i\n", GetScreenWidth(), GetScreenHeight());
        Vector2 mouseWheel = GetMouseWheelMoveV();
        Vector2 mousePosition = GetMousePosition();

        Clay_RenderCommandArray renderCommands = UpdateDrawFrame(
            (float)GetScreenWidth(), (float)GetScreenHeight(),
            mouseWheel.x, mouseWheel.y, mousePosition.x, mousePosition.y,
            IsTouchButtonDown(), IsMouseButtonDown(0), false, false, false, 0.1);

        BeginDrawing();
        ClearBackground(BLACK);
        Clay_Raylib_Render(renderCommands);
        EndDrawing();
    }
#endif // PLATFORM_DESKTOP
    return 0;
}

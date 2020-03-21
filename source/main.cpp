#include <citro2d.h>
#include <3ds.h>
#include <climits>
#include <time.h>
#include <string>
#include "level.h"

const int SCREEN_WIDTH_TOP = 400;
const int SCREEN_WIDTH_BOTTOM = 320;
const int SCREEN_HEIGHT = 240;
const float DEG2RAD = 0.01745329251f;
const u32 COLOUR_WHITE = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);

C2D_TextBuf staticTextBuf, dynamicTextBuf;

u32 lastFrame = 0;
bool isPaused = false;
bool darkOverlay = false;
float score = 0;
GameState state = GameState::PreGame;
Level level;

void changeState(GameState s);
void drawText(C2D_Text text, float x, float y, float xAnchor, float scaleX, float scaleY, u32 colour);
C2D_Text createText(C2D_TextBuf buf, const char* str);

int main(int argc, char *argv[]) {
	// Init libs
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	// Create screen render targets
	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	u32 colourBackground = C2D_Color32(0x2C, 0x2C, 0x2C, 0xFF);

	staticTextBuf = C2D_TextBufNew(4096);
	dynamicTextBuf = C2D_TextBufNew(4096);

	C2D_Text textTitle = createText(staticTextBuf, "F L U C T U S");
	C2D_Text textStart = createText(staticTextBuf, "< Press A To Play >");
	C2D_Text textReplay = createText(staticTextBuf, "< Press A To Replay >");
	C2D_Text textQuit = createText(staticTextBuf, "Select: QUIT");
	C2D_Text textPause = createText(staticTextBuf, "Start: PAUSE");

	changeState(GameState::PreGame);
	level.spawnWave();

	while (aptMainLoop()) {
		// No deltaTime, can't get u64 to convert to u32 without fucking up
		// int t = osGetTime() % INT32_MAX;
		// int frames = t - lastFrame;
        // float deltaTime = frames * 0.001f; // t is in milliseconds, convert to seconds
		// lastFrame = t;

		// 3ds will nearly always run at 60fps so using a constant is probably fine for now
		float dt = 0.016f;
		if(isPaused) dt = 0;

		darkOverlay = isPaused || state != GameState::InGame;

		char scoreChars[64];
		snprintf(scoreChars, sizeof(scoreChars), "%08d", (int) score);

		// Check user input
		hidScanInput();
		u32 keyDown = hidKeysDown();
		if (keyDown & KEY_SELECT)
			break; // break in order to return to hbmenu
		if (keyDown & KEY_START) {
			isPaused = !isPaused;
		}
		if(keyDown & KEY_A || keyDown & KEY_B || keyDown & KEY_X || keyDown & KEY_Y) {
			if(state == GameState::InGame) {
				level.player->flip();
			} else {
				changeState(GameState::InGame);
			}
		}

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, colourBackground);
		C2D_TargetClear(bottom, colourBackground);
		C2D_TextBufClear(dynamicTextBuf);
		consoleClear();

		C2D_SceneBegin(top);

		level.update(dt);
		level.render();

		if(darkOverlay) {
			C2D_DrawRectSolid(0, 0, 0, SCREEN_WIDTH_TOP, SCREEN_HEIGHT, C2D_Color32(0, 0, 0, 0x80));
		}

		if(state == GameState::PreGame) {
			drawText(textTitle, SCREEN_WIDTH_TOP / 2.0f, 110.0f, 0.5f, 1.0f, 1.0f, C2D_Color32(0xBF, 0xFF, 0xFD, 0xFF));
		}
		else if(state == GameState::PostGame) {
			C2D_Text textScore = createText(dynamicTextBuf, scoreChars);
			drawText(textScore, SCREEN_WIDTH_TOP / 2.0f, 110.0f, 0.5f, 1.0f, 1.0f, COLOUR_WHITE);
		}

		C2D_SceneBegin(bottom);

		if(darkOverlay) {
			C2D_DrawRectSolid(0, 0, 0, SCREEN_WIDTH_BOTTOM, SCREEN_HEIGHT, C2D_Color32(0, 0, 0, 0x80));
		}

		if(state == GameState::PreGame) {
			drawText(textStart, SCREEN_WIDTH_BOTTOM / 2.0f, 100.0f, 0.5f, 1.0f, 1.0f, COLOUR_WHITE);
		} 
		else if(state == GameState::InGame) {
			C2D_Text textScore = createText(dynamicTextBuf, scoreChars);
			drawText(textScore, SCREEN_WIDTH_BOTTOM / 2.0f, 100.0f, 0.5f, 1.0f, 1.0f, COLOUR_WHITE);
		}
		else if(state == GameState::PostGame) {
			drawText(textReplay, SCREEN_WIDTH_BOTTOM / 2.0f, 100.0f, 0.5f, 1.0f, 1.0f, COLOUR_WHITE);
		}

		drawText(textQuit, 5.0f, SCREEN_HEIGHT - 23.0f, 0.0f, 0.7f, 0.7f, COLOUR_WHITE);
		drawText(textPause, SCREEN_WIDTH_BOTTOM - 5.0f, SCREEN_HEIGHT - 23.0f, 1.0f, 0.7f, 0.7f, COLOUR_WHITE);

		C3D_FrameEnd(0);
	}

	level.cleanup();
	C2D_TextBufDelete(dynamicTextBuf);
	C2D_TextBufDelete(staticTextBuf);

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	return 0;
}

void changeState(GameState s) {
	state = s;
	level.onStateChange(state);
	if(state == GameState::InGame) {
		score = 0;
	}
}

void drawText(C2D_Text text, float x, float y, float xAnchor, float scaleX, float scaleY, u32 colour) {
	x -= text.width * xAnchor * scaleX;
	C2D_DrawText(&text, C2D_WithColor, x, y, 0, scaleX, scaleY, colour);
}

C2D_Text createText(C2D_TextBuf buf, const char* str) {
	C2D_Text text;
	C2D_TextParse(&text, buf, str);
	C2D_TextOptimize(&text);
	return text;
}
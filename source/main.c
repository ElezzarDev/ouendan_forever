#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include <citro2d.h>
#include "cwav.h"
#include "beatmap.h"
#include "playback.h"
#include "scoring.h"
#include "graphics.h"
#include <3ds.h>


	//global declarations
	BeatmapNote notes[MAX_NOTES];
	ScoreHistory history[MAX_NOTES]; //stores only the result of the note which was hit at the index of noteIndex

	u16 noteIndex = 0; //stores the index of the last note read  from notes[] array
	
int main(int argc, char **argv)
{
	
	gfxInitDefault(); //initiate screen
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE); //initiate graphics
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C3D_AlphaTest(true, GPU_GREATER, 0);
	C2D_Prepare();
	
	consoleInit(GFX_TOP, NULL); //Initialize console on top screen. Using NULL as the second argument tells the console library to use the internal console structure as current one
	
	C3D_RenderTarget* bottomScreen = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT); //bottom screen select
	u32 darkBlackBackground = C2D_Color32(0, 0, 0, 255);
	
	C2D_SpriteSheet gameSpriteSheet = C2D_SpriteSheetLoad("sdmc:/sprites.t3x");
	noteSpriteY1 = C2D_SpriteSheetGetImage(gameSpriteSheet, 0);

	

	
	if (gameSpriteSheet == NULL) {
		
		printf("ERROR: Could not load texture file from archive!\n");
		
	}

	// check NDSP initialization
	Result ndspResult = ndspInit();
	if (ndspResult == 0) {
		
    printf("NDSP Initialized successfully!\n");
	
	}
	else {
		
		printf("NDSP Initialization failed: 0x%08X\n", (unsigned int)ndspResult);
	
	}

	//setup cwav channels and load files
	CWAV CWAVELeft;
	CWAV CWAVERight;
	CWAV CWAVESFX;
	
	cwavFileLoad(&CWAVELeft, "sdmc:/adpcmL.bcwav", 1);
	cwavFileLoad(&CWAVERight, "sdmc:/adpcmR.bcwav", 1);
	cwavFileLoad(&CWAVESFX, "sdmc:/normalHitclap.bcwav", 1);
	
	printf("cwav left load status: %u\n", CWAVELeft.loadStatus);
	printf("cwav right load status: %u\n", CWAVERight.loadStatus);	
	
	//configure panning and volume for cwav channels
	CWAVESFX.volume = 1.0f;
	CWAVESFX.monoPan = 0.0f;
	
	CWAVELeft.volume = 1.0f;
    CWAVELeft.monoPan = -1.0f;
        
    CWAVERight.volume = 1.0f;
    CWAVERight.monoPan = 1.0f;

	//play the audio!
	cwavPlayResult trackInfo = cwavPlay(&CWAVELeft, 0, -1); //saves result in trackInfo for later use
	cwavPlay(&CWAVERight, 0, -1);
	
	if (cwavIsPlaying(&CWAVELeft) != true) {
		printf("ERROR: left channel not playing :(");
	}


	printf("init steps complete \n");
	
	//parse beatmap...
	
	u16 noteCount = 0; //stores total amount of notes in beatmap	
	parseBeatmap("sdmc:/testBeatmap.bin", notes, &noteCount);

	
	
	touchPosition touch;
	float stylusX = 0.0f;
	float stylusY = 0.0f;
	bool wasTappedThisFrame = false;

	
	// Main loop
	while (aptMainLoop())
	{
		
		hidScanInput();
		u32 kDown = hidKeysDown();

		if (kDown & KEY_START) {
			break; // break in order to return to hbmenu
		}
		
		wasTappedThisFrame = false;
		
		if (kDown & KEY_TOUCH) {

			hidTouchRead(&touch);
			wasTappedThisFrame = true;
			stylusX = touch.px; // apparently its required to use floats for most citro2D functions so this saves performance
			stylusY = touch.py;
			
			printf("\x1b[8;0H touch X:%u   ", touch.px); //dumbass had to add spaces to stop formatting from messing up
			printf("\x1b[9;0H touch Y:%u   ", touch.py);
			
		}
		
		u64 masterClock = querySongTime(trackInfo.monoLeftChannel, CWAVELeft.sampleRate, false);
		
				
		//graphics loop testing
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_SceneBegin(bottomScreen);
		C2D_TargetClear(bottomScreen, darkBlackBackground);
	
		draw_notes(notes, masterClock, &noteCount, &noteIndex);
		ScoreHistory tempScoreHistory = check_score(notes, &noteIndex, masterClock, stylusX, stylusY);
		
		if (tempScoreHistory.result != HIT_NONE && wasTappedThisFrame == true) {
			
			history[noteIndex].result = tempScoreHistory.result;
			noteIndex++;
			
		}
		
		
		//Wait for VBlank
		C3D_FrameEnd(0);
		gspWaitForVBlank();
		
		
	}

	//final cleanup, free the audio buffers before closing.
	
	for (u16 i = 0; i < noteCount; i++) {
		
		if (history[i].result == HIT_300) {printf("HIT 300\n");}
		if (history[i].result == HIT_100) {printf("HIT 100\n");}
		if (history[i].result == HIT_50) {printf("HIT 50\n");}
		if (history[i].result == HIT_MISS) {printf("MISS\n");}
		if (history[i].result == HIT_NONE) {printf("HIT NONE\n");}
		
	}

    printf("\nExecution finished. Press any key to quit.");
	
 
	cwavFileFree(&CWAVELeft);
	cwavFileFree(&CWAVERight);
	cwavFileFree(&CWAVESFX);
    
    // Final delay gate so the window doesn't pop out of existence
    hidScanInput();
    while (hidKeysDown() == 0) {
        hidScanInput();
        gspWaitForVBlank();
    }
	
	

    ndspExit();
    gfxExit();
	return 0;
	
}
	


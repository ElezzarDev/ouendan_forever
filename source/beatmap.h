
/*  This file contains all relevant functions and definitions to beatmap parsing and scoring definitions */

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

//ENGINE CONSTANTS & TIMING DEFINITIONS

#define MAX_NOTES 5000
#define APPROACH_TIME 1000 //this will likely be temporary as it will change at higher difficulties, therefore it will eventually be a variable in main set by the beatmap
#define LINGER_TIME 0 //how many ms after targetTime the note will remain on screen
#define VANISH_TIME 150 //how many ms the note will take to shrink and vanish before the X or 300, 100, or 50 spawn on screen 
#define MESSAGE_LINGER 200 //how long the notes score or miss sprite will remain onscreen in ms
#define HITBOX_RADIUS 20
#define WINDOW_300 40 //time in ms
#define WINDOW_100 80 //time in ms
#define WINDOW_50 120 //time in ms

typedef enum {
    HIT_NONE,
    HIT_MISS,
    HIT_50,
    HIT_100,
    HIT_300
} HitResult;

/*typedef enum {
	CIRCLE,
	SLIDER,
	SPINNER
} NoteType
*/	
	


//note structure

typedef struct {
	//NoteType type;		
    u16 x;               // px screen coordinate X
    u16 y;               // px coordinate Y
    u64 targetTime;  	// note target timestamp in ms
    u8 comboIndex;      // number displayed inside the circle (not yet used)
    u32 circleColor;     // RGBA8 packing visual color info (not yet used)
    HitResult result;    // track score and state (currently replaced by ScoreHistory struct in scoring.h)
    bool isSpawned;      // Playfield visibility tracking (not yet used)
} BeatmapNote;

bool parseBeatmap (const char* filepath, BeatmapNote* noteArray, u16* noteCountOut) {
	
	FILE* file = fopen(filepath, "rb");
	
	if (file == NULL) {
		printf("ERROR: could not read beatmap file at %s\n", filepath);
		printf("press any key to abort.\n");
		while (!hidKeysDown()) {
			hidScanInput();
			gspWaitForVBlank();
		}
		
		ndspExit(); gfxExit();
		return 1;
		
	}
	
	u16 noteCount = 0;
	
	while (noteCount < MAX_NOTES) {
		
		u16 rawX = 0;
		u16 rawY = 0;
		u64 rawTargetTime = 0;
		u8 rawComboIndex = 0;
		u32 rawCircleColor = 0;
		
		
	
		if (fread(&rawX, sizeof(u16), 1, file) != 1) {
			break;
		}
		
		fread(&rawY, sizeof(u16), 1, file);
		fread(&rawTargetTime, sizeof(u64), 1, file);
		fread(&rawComboIndex, sizeof(u8), 1, file);
		fread(&rawCircleColor, sizeof(u32), 1, file);
		
		noteArray[noteCount].x = rawX;
		noteArray[noteCount].y = rawY;
		noteArray[noteCount].targetTime = rawTargetTime;
		noteArray[noteCount].comboIndex = rawComboIndex;
		noteArray[noteCount].circleColor = rawCircleColor;
		noteArray[noteCount].result = HIT_NONE;
		noteArray[noteCount].isSpawned = false;
		
		noteCount++;
		
	}
	
	*noteCountOut = noteCount;
	
	fclose(file);
	
	return true;
	
}

//DEBUG FUNCTION WRITE DUMMY BEATMAP
//writes 10 fake notes spaced 1000ms apart with set values
void writeDummyBeatmap() {
	
	FILE* file = fopen("sdmc:/testBeatmap.bin", "wb");
	
	for (int i = 1; i <= 5; i++) {
	//u16 x1 = 69; u16 y1 = 67; u64 t1 = ((u64)i * 1000); u8 c1 = 1; u32 col1 = 0xFFFFFFFF;
	u16 x1 = 48 * i; u16 y1 = 48 * i; u64 t1 = ((u64) i * 700); u8 c1 = 1; u32 col1 = 0xFFFFFFFF;
    fwrite(&x1, sizeof(u16), 1, file);
    fwrite(&y1, sizeof(u16), 1, file);
    fwrite(&t1, sizeof(u64), 1, file);
    fwrite(&c1, sizeof(u8),  1, file);
    fwrite(&col1, sizeof(u32), 1, file);
	}
	
	printf("generated dummy beatmap!\n");
	
	fclose(file);
	
}

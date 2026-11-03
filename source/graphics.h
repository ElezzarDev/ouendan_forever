#pragma once
#include <stdio.h>
#include <3ds.h>
#include <math.h>
#include <strings.h>
#include <citro2d.h>
#include <citro3d.h>
#include "beatmap.h"

//declare placeholder spritesheet for notes (later format will use multiple sprites to build notes which can display a combo index 1-99
C2D_Image noteSpriteY1;


// Draws shrinking / X animations for recently missed notes.
// Walks backward from the newest resolved note and stops once notes are too old to matter.
void draw_missed_notes(BeatmapNote drawNotes[], u64 clockTime, u16 noteIndex) {

	for (int i = (int)noteIndex - 1; i >= 0; i--) {

		// how long ago the miss happened (ms)
		int64_t age = (int64_t)clockTime - ((int64_t)drawNotes[i].targetTime + LINGER_TIME);
		if (age < 0) age = 0;

		// notes are sorted by time, so everything earlier is even older
		if (age >= VANISH_TIME + MESSAGE_LINGER) break;

		// hit notes in this range just get skipped
		if (drawNotes[i].result != HIT_MISS) continue;

		float cx = drawNotes[i].x;
		float cy = drawNotes[i].y;

		if (age < VANISH_TIME) {
			// shrink phase: scale goes 1.0 -> 0.0, position recomputed from the center
			float scale = 1.0f - (float)age / VANISH_TIME;
			float half = HITBOX_RADIUS * scale;
			if (scale > 0.6f) {
				
				C2D_DrawImageAt(noteSpriteY1, cx - half, cy - half, 0.0f, NULL, scale, scale);
				
			}
		}
		else {
			// X phase
			//C2D_DrawImageAt(missXSprite, cx - HITBOX_RADIUS, cy - HITBOX_RADIUS, MISS_DEPTH, NULL, 1.0f, 1.0f);
		}
	}
}

void draw_notes(BeatmapNote drawNotes[], u64 clockTime, u16 *noteCount, u16 *noteIndex) {

	for (u16 i = *noteIndex; i < *noteCount; i++) {
		
		int64_t timeRemaining = drawNotes[i].targetTime - clockTime;
		
	
		if (timeRemaining > APPROACH_TIME) {
			//optimization to check if no notes are within the spawn range for this frame and break if true
			break;
		}
		
		if (i == *noteIndex && timeRemaining < -LINGER_TIME) {
			//note died of old age and should be counted as a miss. 
			//note shrink function and print miss sprite goes here.
			drawNotes[i].result = HIT_MISS;
			(*noteIndex)++;
			continue;
		}
		
		if (timeRemaining <= APPROACH_TIME && timeRemaining >= -LINGER_TIME) {
			//draws notes which are within the range to be spawned
			C2D_DrawImageAt(noteSpriteY1, (float)drawNotes[i].x - HITBOX_RADIUS, (float)drawNotes[i].y - HITBOX_RADIUS, 0.0f, NULL, 1.0f, 1.0f);
			//highestSpawnedNote = i;
			
			if (timeRemaining >= 0) {
					float progress = (float)timeRemaining / APPROACH_TIME;
					
					//shrinks down natively towards the target bounds
					float currentRadius = HITBOX_RADIUS-2 + (72.0f * progress); 

					u32 ringColor = C2D_Color32(200, 200, 20, 255);
					u32 ringColor2 = C2D_Color32(255, 255, 255, 1);
					C2D_DrawCircle(drawNotes[i].x, drawNotes[i].y, 0.2f, currentRadius -3, ringColor2, ringColor2, ringColor2, ringColor2);
					C2D_DrawCircle(drawNotes[i].x, drawNotes[i].y, 0.1f, currentRadius, ringColor, ringColor, ringColor, ringColor);
			
			}
			
		}
		
	}
	
	draw_missed_notes(drawNotes, clockTime, *noteIndex);
}


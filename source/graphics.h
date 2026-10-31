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
			//code to pass miss information to main goes here.
			//need a way to set history to HIT_MISS in this function still.
			(*noteIndex)++;
			continue;
		}
		
		if (timeRemaining <= APPROACH_TIME && timeRemaining >= -LINGER_TIME) {
			//draws notes which are within the range to be spawned
			C2D_DrawImageAt(noteSpriteY1, (float)drawNotes[i].x - HITBOX_RADIUS, (float)drawNotes[i].y - HITBOX_RADIUS, 0.3f, NULL, 0.75f, 0.75f);
			//highestSpawnedNote = i;
			
			if (timeRemaining >= 0) {
					float progress = (float)timeRemaining / APPROACH_TIME;
					
					//shrinks down natively towards the target bounds
					float currentRadius = HITBOX_RADIUS + (72.0f * progress); 

					u32 ringColor = C2D_Color32(200, 200, 20, 255);
					u32 ringColor2 = C2D_Color32(255, 255, 255, 1);
					C2D_DrawCircle(drawNotes[i].x, drawNotes[i].y, 0.2f, currentRadius -3, ringColor2, ringColor2, ringColor2, ringColor2);
					C2D_DrawCircle(drawNotes[i].x, drawNotes[i].y, 0.1f, currentRadius, ringColor, ringColor, ringColor, ringColor);
			
			}
			
		}
		
	}
}

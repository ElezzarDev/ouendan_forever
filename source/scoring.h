#pragma once
#include "beatmap.h"
#include "cwav.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <3ds.h>

//this struct will hold more data later and is currently a placeholder
typedef struct {
	HitResult result;
} ScoreHistory;



//take the x & y, note array and note index and check it with the function and return the hit result
ScoreHistory check_score(BeatmapNote notesArray[], u16 *noteIndex, u64 clockTime, float stylusX, float stylusY) {
	int64_t inputTimeRemaining = (int64_t)notesArray[*noteIndex].targetTime - clockTime;
	ScoreHistory score;
	score.result = HIT_NONE;

	
	if (inputTimeRemaining <= APPROACH_TIME && inputTimeRemaining >= -LINGER_TIME) {
		
		float dx = stylusX - notesArray[*noteIndex].x;
		float dy = stylusY - notesArray[*noteIndex].y;
		float distance = sqrtf(dx * dx + dy * dy);
		
		if (distance <= HITBOX_RADIUS) {
			int64_t absTime = llabs(inputTimeRemaining);
			
			
			if (absTime <= WINDOW_300) {
				
				score.result = HIT_300;
				
			}
			else if (absTime <= WINDOW_100) {
				
				score.result = HIT_100;
					
			}
			else if (absTime <= WINDOW_50) {
				
				score.result = HIT_50;
					
			} 
			else {
				
				score.result = HIT_MISS;
				
				
			}		
		}
	}
	
	return score;
	
}
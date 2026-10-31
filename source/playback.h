#pragma once

#include <3ds.h>
#include <stdio.h>


//this contains all helper functions related to audio playback and memory allocation. its probably unnecessary but it will make things look slightly cleaner and more readable in main
//largely empty since switched over to cwav.



u64 querySongTime(int channel, u32 sampleRate, bool printToConsole) {
	
	u32 currentSample = ndspChnGetSamplePos(channel);
	u64 timeMs = ((u64)currentSample * 1000) / sampleRate;
	
	if (printToConsole == true)  {
		printf("song time: %llu\n", timeMs);
	}
	
	return timeMs;
}

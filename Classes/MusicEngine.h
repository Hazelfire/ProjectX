#pragma once
#include <string>
#include "Parser/Music.h"
#include "MyMath.h"

using namespace std;

class Music {
public:
	static void play(string filename);
	static void playRegion(string region);
	static void playRegion(Vec2i coordinates);
private:
	static string findSongWithRegion(string region);
	static string currentSong;
	static double currentVolume;
};
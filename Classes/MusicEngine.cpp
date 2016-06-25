#include "MusicEngine.h"
#include "SimpleAudioEngine.h"
#include "cocos2d.h"
#include "ResourceMacros.h"
#include "ScriptLoader.h"
#include "Arena.h"

#define MUSIC_TRANSITION_SPEED 0.01

string Music::currentSong;
void Music::play(string filename) {
	auto audio = CocosDenshion::SimpleAudioEngine::getInstance();
	string prefix = MUSIC_FOLDER;
	string suffix =".wav";
	string fullPath = prefix + filename + suffix;
	//CCLOG(fullPath.c_str());
	if (fullPath.compare(currentSong) != 0) {
		audio->stopBackgroundMusic();
		currentSong = fullPath;
		audio->playBackgroundMusic(fullPath.c_str(), true);
		return;
	}
}
void Music::playRegion(string region) {
	string song = findSongWithRegion(region);
	if (!song.empty()) {
		play(song);
	}
}

void Music::playRegion(Vec2i coordinates) {
	playRegion(Arena::getMapInstance()->getTileRegionAt(coordinates));
}

string Music::findSongWithRegion(string region) {
	static const MusicData musicInformation = MusicParser::parse(ScriptLoader::loadXmlScript(ScriptLoader::XML_MUSIC));
	auto requirements = musicInformation.requirements;
	for (list<Requirement>::iterator requirementIterator = requirements.begin(); requirementIterator != requirements.end(); requirementIterator++) {
		Requirement currentRequirement = *requirementIterator;
		if (currentRequirement.region == region) {
			return currentRequirement.songs.front();
		}
	}
	return "";
}
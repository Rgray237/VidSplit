#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>

struct track
{
	std::string trackName, startTime, duration;
	int trackNumber;
};

class VidSplitter
{
public:
	VidSplitter(std::string fileName);
	~VidSplitter();
	bool doesFileExist(std::string filename);
	std::string createThumbnailImage();
	void createTrack(std::string trackName, int number, std::string startTime, std::string duration);
	bool isTrackTimeValid(std::string startTime, std::string duration);
	std::string getFileDurationString();
	double getFileDurationSecs();
	std::string getWorkingDirectory();
	std::string getInstallDirectory();
	void setArtistName(std::string);
	void setWorkingDirectory(std::string path);
	void setOutputFileType(int type);//0 - .mp3, 1 -- .mp4; 2 -- .mov
	void cancelTrackCreation();
	int vsTypeMp3() { return 0; }
	int vsTypeMp4() { return 1; }
	int vsTypeMov() { return 2; }
	int vsTypeAVI() { return 3; }

	void writeOut(std::string str);

private:
	std::ofstream outFileStream;
	std::vector<track> tracksToAdd;
	int numtracks;
	double fileStreamDuration = 0;
	bool isVideoFile;
	bool currentProcessIsTrackCreation = false;
	int outFileType = 0;
	int processId = -1;
	std::string inVidFileName, inAudioFileName, outDirectoryPath, thumbNailFileName, outFileName;
	std::string metaArtistName;
	std::string inVidDirectory;
	std::string installDirectory;
	std::string ffmpegPath;
	void createTrackFromTime_ForDuration(std::string startTime, std::string duration, std::string inFile, std::string outFile, int number);

	void probeFileForInfo();
	void createSimpleProcess(std::string cmd);
	void kill_by_pid(int pid);
	double timeStringToDouble(std::string time);
	double getFileDuration();
	void setDefaultWorkingDirectory();
	std::string getOutFileTypeAsExtensionString();

	void log(std::string str);
};
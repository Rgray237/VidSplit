#pragma once
#include "VidSplit.h"
#include <string>
#include <msclr\marshal_cppstd.h>
using namespace System;

namespace CLRWrapper {

	public ref class VidSplitWrapper
	{
	public:
		//ctor
		VidSplitWrapper(String^ fileName);
		~VidSplitWrapper();
		//methods
		bool doesFileExist(String^ fileName);
		String^ createThumbnailImage();
		bool createTrack(String^ trackName, int number, String^ startTime, String^ duration);
		bool isTrackTimeValid(String^ startTime, String^ duration);
		String^ getFileDuration();
		String^ getWorkingDirectory();
		String^ getInstallDirectory();
		void setWorkingDirectory(String^ newDirectoryName);
		double getFileDurationSecs();
		void setArtistName(String^ str);

		void setOutputFileType(int type);//0 - .mp3, 1 -- .mp4;

		void cancelTrackCreation() { return vidSplit->cancelTrackCreation(); }
		void writeOut(String^ str);
		int vsTypeMp3() { return vidSplit->vsTypeMp3(); }
		int vsTypeMp4() { return vidSplit->vsTypeMp4(); }
		int vsTypeMov() { return vidSplit->vsTypeMov(); }
		int vsTypeAVI() { return vidSplit->vsTypeAVI(); }
	private:
		VidSplitter * vidSplit;
	};

}
#include "stdafx.h"

#include "VidSplitWrapper.h"
#include "VidSplit.cpp"


CLRWrapper::VidSplitWrapper::VidSplitWrapper(String^ fileName)
{
	msclr::interop::marshal_context context;
	std::string str = context.marshal_as<std::string>(fileName);
	vidSplit = new VidSplitter(str);
}

bool CLRWrapper::VidSplitWrapper::doesFileExist(String^ inVidFileName)
{
	msclr::interop::marshal_context context;
	std::string str = context.marshal_as<std::string>(inVidFileName);
	return vidSplit->doesFileExist(str);
}


bool CLRWrapper::VidSplitWrapper::isTrackTimeValid(String^ startTime, String^ duration)
{
	msclr::interop::marshal_context context;
	std::string str1 = context.marshal_as<std::string>(startTime);
	std::string str2 = context.marshal_as<std::string>(duration);
	return vidSplit->isTrackTimeValid(str1, str2);
}

String^ CLRWrapper::VidSplitWrapper::getFileDuration()
{
	std::string str = vidSplit->getFileDurationString();

	msclr::interop::marshal_context context;
	String^ ret = context.marshal_as<String^>(str);
	return ret;
}

double CLRWrapper::VidSplitWrapper::getFileDurationSecs()
{
	return vidSplit->getFileDurationSecs();
}


String^ CLRWrapper::VidSplitWrapper::createThumbnailImage()
{
	msclr::interop::marshal_context context;
	String^ ret = context.marshal_as<String^>(vidSplit->createThumbnailImage());
	return ret;
}

bool CLRWrapper::VidSplitWrapper::createTrack(String^ trackName, int number, String^ startTime, String^ duration)
{
	msclr::interop::marshal_context context;
	std::string trackNameStr = context.marshal_as<std::string>(trackName);
	std::string startTimeStr = context.marshal_as<std::string>(startTime);
	std::string durationStr = context.marshal_as<std::string>(duration);
	vidSplit->createTrack(trackNameStr, number, startTimeStr, durationStr);
	return true;
}

String^ CLRWrapper::VidSplitWrapper::getWorkingDirectory()
{
	std::string str = vidSplit->getWorkingDirectory();

	msclr::interop::marshal_context context;
	String^ ret = context.marshal_as<String^>(str);
	return ret;
}
String^ CLRWrapper::VidSplitWrapper::getInstallDirectory()
{
	std::string str = vidSplit->getInstallDirectory();

	msclr::interop::marshal_context context;
	String^ ret = context.marshal_as<String^>(str);
	return ret;
}

void CLRWrapper::VidSplitWrapper::writeOut(String^ str)
{
	msclr::interop::marshal_context context;
	std::string ret = context.marshal_as<std::string>(str);
	vidSplit->writeOut(ret);
}

void CLRWrapper::VidSplitWrapper::setWorkingDirectory(String^ str)
{
	msclr::interop::marshal_context context;
	std::string ret = context.marshal_as<std::string>(str);
	vidSplit->setWorkingDirectory(ret);
}

void CLRWrapper::VidSplitWrapper::setOutputFileType(int type)
{
	vidSplit->setOutputFileType(type);
}

void CLRWrapper::VidSplitWrapper::setArtistName(String^ str)
{
	msclr::interop::marshal_context context;
	std::string ret = context.marshal_as<std::string>(str);
	vidSplit->setArtistName(ret);
}

CLRWrapper::VidSplitWrapper::~VidSplitWrapper()
{
	delete vidSplit;
}
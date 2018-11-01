#include "VidSplit.h"

#include <iostream>
#include <fstream>
#include <regex>
#include <Windows.h>
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>
#include <processthreadsapi.h>
#include <experimental\filesystem>

#define INBUF_SIZE 4096
//
//void clearLog()
//{
//	std::ofstream fs;
//	fs.open("logout.txt");
//	fs.clear();
//}

void VidSplitter::log(std::string str)
{
	outFileStream << str << std::endl;
}


VidSplitter::VidSplitter(std::string fileName)
{
	//clearLog();
	outFileStream.open("logout.txt");

	if (doesFileExist(fileName))
	{
		isVideoFile = true;
		VidSplitter::inVidFileName = fileName;
		setDefaultWorkingDirectory();
		probeFileForInfo();
		VidSplitter::ffmpegPath = installDirectory + "\\ffmpeg\\bin\\";
		size_t mp3it = fileName.find(".mp3");
		size_t wavit = fileName.find(".wav");
		if (mp3it != std::string::npos || wavit != std::string::npos)
			isVideoFile = false;
		log( "VidSplitter Properly Constructed");
	}
	else
	{
		inVidFileName = "";
		std::cout << "Improper Construction" << std::endl;
	}
}

VidSplitter::~VidSplitter()
{
	//log("Destroying");
	remove("thumbOut.bmp");
	outFileStream.close();
	/*std::ifstream inFile("temp\\fileInfo.txt");
	if (inFile.is_open())
	{
		std::cout << "FILE STILL OPEN... closing." << std::endl;
		inFile.close();
	}*/


}

void VidSplitter::writeOut(std::string str)
{
	//log(str);
	std::cout << str << std::endl;
}

void VidSplitter::kill_by_pid(int pid)
{
	log("Kill process");
	HANDLE handy;
	handy = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, TRUE, pid);
	std::cout << TerminateProcess(handy, 0) << std::endl;

}

void VidSplitter::setArtistName(std::string artistName)
{
	log("Set artist name");
	if (artistName != "" && outFileType == vsTypeMp3())
	{
		metaArtistName = " -metadata album_artist=\"" + artistName + "\"";
	}
	else
		metaArtistName = "";
}

void VidSplitter::cancelTrackCreation()
{
	log("cancelTrackCreation");
	if (processId == -1 || !currentProcessIsTrackCreation)
	{
		std::cout << "NO PROCESS ID" << std::endl;
		return;
	}
	log("Kill process" + std::to_string(processId));
	kill_by_pid(processId);

	if (doesFileExist(outFileName))
	{
		if (remove(outFileName.c_str()) != 0)
			std::cout << "Error deleting file" << std::endl;
	}
	else
		std::cout << "File deleted" << std::endl;
}


void VidSplitter::setDefaultWorkingDirectory()
{
	log("setDefaultWorkingDirectory");

	std::experimental::filesystem::path pth(inVidFileName);
	VidSplitter::inVidDirectory = pth.parent_path().generic_string();
	VidSplitter::outDirectoryPath = inVidDirectory;

	unsigned long nBufferLength = 1000;
	char * lpBuffer = new char[nBufferLength];
	unsigned long ret = GetCurrentDirectory(nBufferLength, lpBuffer);

	VidSplitter::installDirectory = std::string(lpBuffer);
	delete[] lpBuffer;
	log("installdirectory = " + installDirectory);

}

void VidSplitter::setWorkingDirectory(std::string path)
{

	VidSplitter::outDirectoryPath = path;
	log("outDirectoryPath = " + path);
}

std::string VidSplitter::getWorkingDirectory()
{
	return outDirectoryPath;
}


void VidSplitter::probeFileForInfo()
{
	std::string cmdText = ffmpegPath + "ffprobe -v error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 \"" + inVidFileName + "\" > \"fileInfo.txt\"" + " 2>&1";
	system(cmdText.c_str());
	std::cout << cmdText << std::endl;
	log(cmdText);
	fileStreamDuration = getFileDuration();
	return;
}

void VidSplitter::setOutputFileType(int type)
{
	switch (type)
	{
	case 0:
		outFileType = vsTypeMp3();
		break;
	case 1:
		outFileType = vsTypeMp4();
		break;
	case 2:
		outFileType = vsTypeMov();
		break;
	case 3:
		outFileType = vsTypeAVI();
		break;
	default:
		outFileType = vsTypeMp3();
		break;
	}
}

std::string VidSplitter::getFileDurationString()
{
	int hrs, mins, secs;
	hrs = (int)fileStreamDuration / (60 * 60);
	mins = (int)fileStreamDuration / 60 - hrs * 60;
	secs = (int)fileStreamDuration - mins * 60 - hrs * 60;

	std::string hrstr = "", minstr = "", secstr = "";
	if (hrs < 10)
		hrstr = "0";
	if (mins < 10)
		minstr = "0";
	if (secs < 10)
		secstr = "0";
	hrstr = hrstr + std::to_string(hrs);
	minstr = minstr + std::to_string(mins);
	secstr = secstr + std::to_string(secs);

	std::string str = hrstr + ":" + minstr + ":" + secstr;
	return str;
}

double VidSplitter::getFileDurationSecs()
{
	return fileStreamDuration;
}

double VidSplitter::getFileDuration()
{
	double x = 0;
	std::cout << "GetFileDuration" << std::endl;


	std::ifstream in;
	in.open("fileInfo.txt");
	if (!in)
	{
		log("Could not open fileInfo.Txt");
		return x;
	}
	in >> x;
	std::cout << "Duration= " << x << std::endl;
	log("Duration= " + std::to_string(x));
	in.close();
	std::cout << "Removing File" << std::endl;
	log("removing fileInfo.Txt");

	if (remove("fileInfo.txt") != 0)
		log("Error deleting fileInfo.txt");
	else
		log("File removed");
	return x;
}

std::string VidSplitter::getOutFileTypeAsExtensionString()
{
	switch (outFileType)
	{
	case 0:
		return ".mp3";
	case 1:
		return ".mp4";
	case 2:
		return ".mov";
	case 3:
		return ".avi";
	default:
		return ".mp3";
	}
}

bool VidSplitter::isTrackTimeValid(std::string startTime, std::string duration)
{
	log("checking track time validity: start = " +startTime + " " + duration);
	if (std::stod(startTime.substr(6, 2)) > 59 || std::stod(startTime.substr(3, 2)) > 59 || std::stod(duration.substr(6, 2)) > 59 || std::stod(duration.substr(3, 2)) > 59)
		return false;

	double startTimeSeconds = timeStringToDouble(startTime);
	double DurationSeconds = timeStringToDouble(duration);
	log("Start time is = " + std::to_string(startTimeSeconds) + " and duration is " + std::to_string(fileStreamDuration));
	if (DurationSeconds <= 0 || startTimeSeconds<0 || startTimeSeconds>fileStreamDuration || startTimeSeconds + DurationSeconds > fileStreamDuration) // Error time conditions. Zero or negative track duration, start Time later than file length, track start + track duration later than file duration.
		return false;
	std::cout << "Start Time seconds = " << startTimeSeconds << " Duration seconds = " << DurationSeconds << std::endl;
	return true;
}



void VidSplitter::createSimpleProcess(std::string cmd)
{
	LPCSTR                lpApplicationName;
	LPSTR                 lpCommandLine;
	LPSECURITY_ATTRIBUTES lpProcessAttributes;
	LPSECURITY_ATTRIBUTES lpThreadAttributes;
	BOOL                  bInheritHandles;
	DWORD                 dwCreationFlags;
	LPVOID                lpEnvironment;
	LPCSTR                lpCurrentDirectory;
	STARTUPINFO        si;
	PROCESS_INFORMATION pi;

	lpCommandLine = const_cast<char *>(cmd.c_str());

	std::cout << "CREATING PROCESS with CMD: " << cmd << std::endl;
	log("CREATING PROCESS with CMD: " + cmd);
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	std::cout << "Creating process now." << std::endl;
	if (!CreateProcess(NULL,
		lpCommandLine,
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi))           // Pointer to PROCESS_INFORMATION structure
	{
		std::cout << "Create Process failed" << GetLastError() << std::endl;
		return;
	}
	std::cout << "Process Created. " << pi.dwProcessId << std::endl;
	processId = pi.dwProcessId;
	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, 3000);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);

	std::cout << "Process ended. " << processId << std::endl;
	log("PROCESS ENDED. " + std::to_string(processId));
	CloseHandle(pi.hThread);
	processId = -1;
}


void VidSplitter::createTrackFromTime_ForDuration(std::string startTime, std::string duration, std::string inFile, std::string outFile, int number)
{
	log("createAudioTrackFromTime_ForDuration");
	if (doesFileExist(inFile))
	{
		if (isTrackTimeValid(startTime, duration))
		{
			outFileName = outFile;
			std::string cmdText, start, dur;
			start = startTime;
			dur = duration;
			cmdText = ffmpegPath + std::string("ffmpeg -y -ss ") + start + " -i " + "\"" + inVidFileName + "\"" + metaArtistName + " -metadata track=" + std::to_string(number) + " -t " + dur + " -b:a 160K " + "\"" + outFile + "\"";
			//cmdText = " -y -ss " + start + " -i " + inVidFileName + " -t " + dur + " -b:a 160K " + outFile;

			//system(cmdText.c_str());

			currentProcessIsTrackCreation = true;
			createSimpleProcess(cmdText);
		}
		else
			std::cout << "Track time/duration is invalid for track: " << outFile << std::endl;
	}
	else
		std::cout << inFile << "  Does not Exist" << std::endl;
}

bool VidSplitter::doesFileExist(std::string fileName)
{
	if (std::experimental::filesystem::exists(fileName) || std::experimental::filesystem::exists(fileName + VidSplitter::getOutFileTypeAsExtensionString()))
	{
		return true;
	}
	return false;
}

std::string VidSplitter::getInstallDirectory()
{
	return installDirectory;
}


std::string VidSplitter::createThumbnailImage()
{
	if (doesFileExist(inVidFileName) && isVideoFile)
	{
		std::string cmdText;
		thumbNailFileName = installDirectory + "\\thumbOut.BMP";
		int secs = (int)getFileDurationSecs();
		cmdText = ffmpegPath + std::string("ffmpeg -y -i ") + "\"" + inVidFileName + "\"" + " -ss " + std::to_string(secs) + " -vframes 1 \"" + thumbNailFileName + "\"";//ffmpegPath+ std::string("ffmpeg -y -i ") + "\"" + inVidFileName +"\""+ " -ss 00:00:00 -vframes 1 \""+thumbNailFileName+"\"";
		createSimpleProcess(cmdText);
		currentProcessIsTrackCreation = false;
		return thumbNailFileName;
	}
	return "";
}




void VidSplitter::createTrack(std::string trackName, int number, std::string startTime, std::string duration)
{
	std::string fullTrackPath = outDirectoryPath + "\\" + trackName + getOutFileTypeAsExtensionString();
	createTrackFromTime_ForDuration(startTime, duration, inVidFileName, fullTrackPath, number);
}

double VidSplitter::timeStringToDouble(std::string time)
{
	double timeInSeconds = std::stod(time.substr(0, 2)) * 60 * 60 + std::stod(time.substr(3, 2)) * 60 + std::stod(time.substr(6, 2));
	return timeInSeconds;
}



int main()
{
	return 0;
}
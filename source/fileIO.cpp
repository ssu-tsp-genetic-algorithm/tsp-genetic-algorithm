#include "fileIO.h"

IOManager::IOManager()
{
	
}

IOManager::IOManager(const string newLocation)
{
	SetFileLocation(newLocation);
}

IOManager::~IOManager()
{
	if(readTargetFile.is_open())
		readTargetFile.close();
	
	if(writeTargetFile.is_open())
		writeTargetFile.close();
}

	
bool IOManager::Initialize(string newLocation)
{
	if(!SetFileLocation(newLocation)) return false;
	
	readTargetFile.open(location);
	writeTargetFile.open(location);
	
	return true;
}

bool IOManager::SetFileLocation(string newLocation)
{
	if(newLocation == "") return false;
	location = newLocation;
	return true;
}

void IOManager::PrintToFile(string str)
{
	if(!writeTargetFile.is_open()) return;

	writeTargetFile<<str;
}

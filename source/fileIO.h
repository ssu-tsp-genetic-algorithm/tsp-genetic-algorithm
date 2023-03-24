#include <fstream>
#include <string>
using namespace std;

class IOManager
{
public:
	IOManager();

	IOManager(const string newLocation);

	~IOManager();
public:
	bool Initialize(string newLocation);

	bool SetFileLocation(string newLocation);

	void PrintToFile(string str);

private:
	std::ifstream readTargetFile;

	std::ofstream writeTargetFile;

	string location;
};
#include "QuaverFile.h"

void Average4k::Chart::QuaverFile::Parse()
{
	// load the file (from path), and check if it loaded correctly
	std::ifstream infile(path);

	if (!infile.good())
	{
		AvgEngine::Logging::writeLog("[QuaverFile] [Error] Couldn't load " + path + "!");
		return;
	}


}

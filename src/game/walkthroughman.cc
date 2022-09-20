#include "game\walkthroughman.hh"
#include "exceptions.hh"

void WalkthroughMan::NextLevel(Level *level) {
	char path[16];
	EASSERT(std::snprintf(path, 16, "levels\\%03d.dat", ++m_dwCurrLevel) > 0);
	level->Load(path);	
}

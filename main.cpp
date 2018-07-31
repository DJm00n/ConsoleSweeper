#include "Game.h"

#include <iostream>

#ifdef _MSC_VER
#include <sstream>
#include <Windows.h>

class CBuf : public std::stringbuf
{
public:
	int sync() override
	{
		fputs(str().c_str(), stdout);
		str("");
		return 0;
	}
};
#endif //_MSC_VER

int main()
{
#ifdef _MSC_VER
	// Crunch for UTF-8 output on Windows
	// note: files must be in UTF-8-BOM encoding!
	// https://stackoverflow.com/a/45588456
	SetConsoleOutputCP(CP_UTF8);
	setvbuf(stdout, nullptr, _IONBF, 0);
	CBuf buf;
	std::cout.rdbuf(&buf);
#endif //_MSC_VER

    Game game({10, 10}, 5); // 10x10 field and 5 mines and flags
    game.start();

    std::cout << "Press enter to continue!\n";
    std::cin.ignore();
    
    return 0;
}

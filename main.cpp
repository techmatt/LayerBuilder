
#include "main.h"

class App
{
public:
	void go();
};

void App::go()
{
	Console::openLogFile("console.txt");

	
	Console::log("logging!");
	std::cin.get();
}

void main()
{
	App a;
	a.go();
}

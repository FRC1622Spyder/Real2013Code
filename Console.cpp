#include "Console.h"

Console::Console() : Spyder::Subsystem("console")
{
	this->SetPeriod(50);
}

void Console::Init(Spyder::RunModes runmode)
{
}

void Console::Periodic(Spyder::RunModes runmode)
{
}

void Console::RobotInit()
{
}

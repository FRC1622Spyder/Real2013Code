#pragma once
#define PATH_MAX 255
#include <WPILib.h>

namespace Spyder
{
	Joystick *GetJoystick(UINT32 port);
	Victor* GetVictor(UINT32 channel);
	Solenoid* GetSolenoid(UINT32 channel);
	double GetDeadzone();
};

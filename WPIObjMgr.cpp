#include "WPIObjMgr.h"

namespace Spyder
{
	Joystick *GetJoystick(UINT32 port)
	{
		static Joystick* s_joysticks[] = {0, 0, 0, 0};
		if(!s_joysticks[port])
		{
			s_joysticks[port] = new Joystick(port);
		}
		return s_joysticks[port];
	}
}

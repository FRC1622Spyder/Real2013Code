#pragma once
#include "Subsystem.h"

class Console : public Spyder::Subsystem
{
	private:
		Console();
	public:
		virtual void Init(Spyder::RunModes runmode);
		
		virtual void Periodic(Spyder::RunModes runmode);
		
		virtual void RobotInit();
};

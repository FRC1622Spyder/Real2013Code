#include "Subsystem.h"
#include "WPIObjMgr.h"
#include "Config.h"
#include "Vision/RGBImage.h"
#include "Vision/BinaryImage.h"
#include <WPILib.h>
#include "Math.h"

class Target : public Spyder::Subsystem
{
	float relX;
	float relY;
	
	//AxisCamera &camera;
	RGBImage *image;
	
public:
	Target() : Spyder::Subsystem("Target")
		{
		AxisCamera &camera = AxisCamera::GetInstance("10.16.22.11");
		RGBImage *image = new RGBImage("init.png"); 
		
		}
	virtual ~Target()
	{
	}
	
	virtual void Init()
	{
	}
	
	virtual void Periodic(Spyder::RunModes runmode)
		{
			switch(runmode)
			{
			case Spyder::M_DISABLED:
				//do something
				break;
			case Spyder::M_TELEOP:
				//do something
				break;
			case Spyder::M_AUTO:
				//do something
				break;
			default:
				//do something
			}
		}

	virtual void RobotInit()
	{
	}
	
};
Target target;

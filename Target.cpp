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
	time_t lastImage;
	
	RGBImage *image;
	
public:
	Target() : Spyder::Subsystem("Target")
		{

		
		RGBImage *image = new RGBImage("init.png"); 
		
		}
	virtual ~Target()
	{
	}
	
	 void Init(Spyder::RunModes runmode)
	{
		
	}
	
	void Periodic(Spyder::RunModes runmode)
		{
			switch(runmode)
			{
			case Spyder::M_DISABLED:
				//do something
				break;
			case Spyder::M_TELEOP:
				
				break;
			case Spyder::M_AUTO:
				//do something
				break;
			default:
				std::cout << "oops" << std::endl;
				//do something
			}
		}

	void RobotInit()
	{
	}
	
};
Target target;

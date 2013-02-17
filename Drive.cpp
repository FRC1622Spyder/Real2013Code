#include "Subsystem.h"
#include "WPIObjMgr.h"
#include "Config.h"
#include <math.h>

class Drive : public Spyder::Subsystem
{
	private:
		Spyder::TwoIntConfig leftJoystick;
		Spyder::TwoIntConfig rightJoystick;
		
		Spyder::ConfigVar<UINT32> leftMotor;
		Spyder::ConfigVar<UINT32> rightMotor;
		
		Spyder::ConfigVar<bool> leftMotorInv;
		Spyder::ConfigVar<bool> rightMotorInv;
	public:
		Drive() : Spyder::Subsystem("Drive"), leftJoystick("bind_leftDrive", 1, 1),
			rightJoystick("bind_rightDrive", 2, 1), leftMotor("leftDriveMotor", 2),
			rightMotor("rightDriveMotor", 1), leftMotorInv("leftDriveInverted", false),
			rightMotorInv("rightDriveInverted", false)
		{
		}
		
		virtual ~Drive()
		{
		}
		
		virtual void Init(Spyder::RunModes runmode)
		{
		}
		
		virtual void Periodic(Spyder::RunModes runmode)
		{
			switch(runmode)
			{
				case Spyder::M_DISABLED:
					Spyder::GetVictor(leftMotor.GetVal())->Set(0);
					Spyder::GetVictor(rightMotor.GetVal())->Set(0);
					break;
				case Spyder::M_TELEOP:
					Joystick *leftJoy = Spyder::GetJoystick(leftJoystick.GetVar(1));
					Joystick *rightJoy = Spyder::GetJoystick(rightJoystick.GetVar(1));
					float left = leftJoy->GetRawAxis(leftJoystick.GetVar(2));
					float right = rightJoy->GetRawAxis(rightJoystick.GetVar(2));
					left = fabs(left) > Spyder::GetDeadzone() ? left : 0;
					right = fabs(right) > Spyder::GetDeadzone() ? right : 0;
					
					if(leftMotorInv.GetVal())
					{
						left *= -1;
					}
					if(rightMotorInv.GetVal())
					{
						right *= -1;
					}
					
					Spyder::GetVictor(leftMotor.GetVal())->Set(left);
					Spyder::GetVictor(rightMotor.GetVal())->Set(right);
					break;
				default:
					Spyder::GetVictor(leftMotor.GetVal())->Set(0);
					Spyder::GetVictor(rightMotor.GetVal())->Set(0);
					break;
			};
		}
		
		virtual void RobotInit()
		{
		}
};

Drive drive;

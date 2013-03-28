#include "Subsystem.h"
#include "WPIObjMgr.h"
#include "Config.h"
#include <math.h>
#include <iostream>

class Drive : public Spyder::Subsystem
{
	private:
		Spyder::TwoIntConfig leftJoystick;
		Spyder::TwoIntConfig rightJoystick;
		
		Spyder::ConfigVar<UINT32> leftMotor;
		Spyder::ConfigVar<UINT32> rightMotor;
		
		Spyder::ConfigVar<bool> leftMotorInv;
		Spyder::ConfigVar<bool> rightMotorInv;
		
		Spyder::TwoIntConfig halfSpeed;
		
		bool reversed;
		bool lastRevBtnVal;
		Spyder::TwoIntConfig reverseBtn;
		
		Spyder::ConfigVar<float> curvature;
	public:
		Drive() : Spyder::Subsystem("Drive"), leftJoystick("bind_leftDrive", 1, 2),
			rightJoystick("bind_rightDrive", 2, 2), leftMotor("leftDriveMotor", 2),
			rightMotor("rightDriveMotor", 1), leftMotorInv("leftDriveInverted", true),
			rightMotorInv("rightDriveInverted", false), halfSpeed("bind_halfSpeedDrive", 1, 1),
			reversed(false), lastRevBtnVal(false), reverseBtn("bind_driveReverse", 2, 3),
			curvature("drive_controlCurvature", 0.351563f)
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
					
					float curve = curvature.GetVal();
					left = ((left*left*left)*curve) + (left*(1.f-curve));
					right = ((right*right*right)*curve) + (right*(1.f-curve));
					
					if(leftMotorInv.GetVal())
					{
						left *= -1;
					}
					if(rightMotorInv.GetVal())
					{
						right*= -1;
					}
					
					if(Spyder::GetJoystick(halfSpeed.GetVar(1))->GetRawButton(halfSpeed.GetVar(2)))
					{
						left /= 2.f;
						right /= 2.f;
					}
					
					if(Spyder::GetJoystick(reverseBtn.GetVar(1))->GetRawButton(reverseBtn.GetVar(2)) && !lastRevBtnVal)
					{
						lastRevBtnVal = true;
						reversed = !reversed;
					}
					lastRevBtnVal = Spyder::GetJoystick(reverseBtn.GetVar(1))->GetRawButton(reverseBtn.GetVar(2));
					
					if(reversed)
					{
						float temp = left;
						left = right;
						right = temp;
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

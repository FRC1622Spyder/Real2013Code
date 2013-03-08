#include "Subsystem.h"
#include "WPIObjMgr.h"
#include "Config.h"
#include <math.h>
#include <iostream>

class Turret : public Spyder::Subsystem
{
	private:
		Spyder::ConfigVar<UINT32> frontMotor;
		Spyder::ConfigVar<UINT32> backMotor;
		
		Spyder::TwoIntConfig turretJoystick;
		
		Spyder::ConfigVar<float> inputMul;
		
		float speed;
		
		Spyder::ConfigVar<bool> frontInv;
		Spyder::ConfigVar<bool> backInv;
		
		Spyder::TwoIntConfig turretUp;
		Spyder::TwoIntConfig turretDown;
		
		Spyder::ConfigVar<UINT32> angMotor;
		Spyder::ConfigVar<float> angSpeed;
		
		Spyder::ConfigVar<UINT32> pistonSolenoidExt;
		Spyder::ConfigVar<UINT32> pistonSolenoidRet;
		Spyder::TwoIntConfig fireButton;
		Spyder::TwoIntConfig stopButton;
	public:
		Turret() : Spyder::Subsystem("Turret"), frontMotor("frontTurretMotor", 4),
			backMotor("backTurretMotor", 3), turretJoystick("bind_turretSpeed", 3, 1),
			inputMul("turretInputMul", 0.1), speed(0.f), frontInv("frontTurretMotorInveted", false),
			backInv("backTurretMotorInveted", false), turretUp("bind_turretUp", 3, 6),
			turretDown("bind_turretDown", 3, 8), angMotor("turret_angMotor", 5),
			angSpeed("turret_angSpeed", 0.1), pistonSolenoidExt("turret_pistonSolenoidExt", 1),
			pistonSolenoidRet("turret_pistonSolenoidRet", 2),
			fireButton("bind_turretFire", 3, 2), stopButton("bind_turretStop", 3, 11)
		{
		}
		
		virtual ~Turret()
		{
		}
		
		virtual void Init(Spyder::RunModes runmode)
		{
			Spyder::GetVictor(frontMotor.GetVal())->Set(0);
			Spyder::GetVictor(backMotor.GetVal())->Set(0);
		}
		
		virtual void Periodic(Spyder::RunModes runmode)
		{
			switch(runmode)
			{
				case Spyder::M_TELEOP:	
					Joystick *joystick = Spyder::GetJoystick(turretJoystick.GetVar(1));
					float val = joystick->GetRawAxis(turretJoystick.GetVar(2)) * -1;
					val = fabs(val) > Spyder::GetDeadzone() ? val : 0;
					val *= inputMul.GetVal();
					speed += val;
					speed = (speed > 1.f) ? 1.f : speed;
					if(val != 0.0f)
					{
						std::cout << "New turret speed: " << speed << std::endl;
					}
					
					if(Spyder::GetJoystick(stopButton.GetVar(1))->GetRawButton(stopButton.GetVar(2)))
					{
						speed = 0;
					}
					
					if(speed < 0)
					{
						speed = 0;
					}
					
					if(frontInv.GetVal())
					{
						Spyder::GetVictor(frontMotor.GetVal())->Set(speed * -1.f);
					}
					else
					{
						Spyder::GetVictor(frontMotor.GetVal())->Set(speed);
					}
					
					if(backInv.GetVal())
					{
						Spyder::GetVictor(backMotor.GetVal())->Set(speed * -1.f);
					}
					else
					{
						Spyder::GetVictor(backMotor.GetVal())->Set(speed);
					}
					
					if(Spyder::GetJoystick(turretUp.GetVar(1))->GetRawButton(turretUp.GetVar(2)))
					{
						Spyder::GetVictor(angMotor.GetVal())->Set(angSpeed.GetVal());
					}
					else if(Spyder::GetJoystick(turretDown.GetVar(1))->GetRawButton(turretDown.GetVar(2)))
					{
						Spyder::GetVictor(angMotor.GetVal())->Set(angSpeed.GetVal() * -1);
					}
					else
					{
						Spyder::GetVictor(angMotor.GetVal())->Set(0);
					}
					
					if(Spyder::GetJoystick(fireButton.GetVar(1))->GetRawButton(fireButton.GetVar(2)))
					{
						Spyder::GetSolenoid(pistonSolenoidExt.GetVal())->Set(true);
						Spyder::GetSolenoid(pistonSolenoidRet.GetVal())->Set(false);
					}
					else
					{
						Spyder::GetSolenoid(pistonSolenoidExt.GetVal())->Set(false);
						Spyder::GetSolenoid(pistonSolenoidRet.GetVal())->Set(true);
					}
						
					break;
				default:
					Spyder::GetVictor(frontMotor.GetVal())->Set(0);
					Spyder::GetVictor(backMotor.GetVal())->Set(0);
					Spyder::GetSolenoid(pistonSolenoidRet.GetVal())->Set(true);
					Spyder::GetSolenoid(pistonSolenoidExt.GetVal())->Set(false);
			}
		}
		
		virtual void RobotInit()
		{
		}
};

Turret turret;

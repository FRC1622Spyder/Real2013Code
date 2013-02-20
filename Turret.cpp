#include "Subsystem.h"
#include "WPIObjMgr.h"
#include "Config.h"
#include <math.h>

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
		
		Spyder::ConfigVar<UINT32> pistonSolenoid;
		Spyder::TwoIntConfig fireButton;
	public:
		Turret() : Spyder::Subsystem("Turret"), frontMotor("frontTurretMotor", 4),
			backMotor("backTurretMotor", 3), turretJoystick("bind_turretSpeed", 3, 1),
			inputMul("turretInputMul", 0.1), speed(0.f), frontInv("frontTurretMotorInveted", false),
			backInv("backTurretMotorInveted", false), turretUp("bind_turretUp", 3, 6),
			turretDown("bind_turretDown", 3, 8), angMotor("turret_angMotor", 5),
			angSpeed("turret_angSpeed", 0.1), pistonSolenoid("turret_pistonSolenoid", 1),
			fireButton("bind_turretFire", 3, 2)
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
					float val = joystick->GetRawAxis(turretJoystick.GetVar(2)) * inputMul.GetVal();
					val = fabs(val) > Spyder::GetDeadzone() ? val : 0;
					speed += val;
					speed = (speed > 1.f) ? 1.f : speed;
					
					Spyder::GetVictor(frontMotor.GetVal())->Set(0);
					Spyder::GetVictor(backMotor.GetVal())->Set(0);
					
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
						Spyder::GetSolenoid(pistonSolenoid.GetVal())->Set(true);
					}
					else
					{
						Spyder::GetSolenoid(pistonSolenoid.GetVal())->Set(false);
					}
						
					break;
				default:
					Spyder::GetVictor(frontMotor.GetVal())->Set(0);
					Spyder::GetVictor(backMotor.GetVal())->Set(0);
			}
		}
		
		virtual void RobotInit()
		{
		}
};

Turret turret;

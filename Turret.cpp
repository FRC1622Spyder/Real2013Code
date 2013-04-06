#include "Subsystem.h"
#include "WPIObjMgr.h"
#include "Config.h"
#include "Console.h"
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
		
		unsigned char autoPhase;
		
		Spyder::TwoIntConfig presetButton;
		double autoStart;
	public:
		Turret() : Spyder::Subsystem("Turret"), frontMotor("frontTurretMotor", 4),
			backMotor("backTurretMotor", 3), turretJoystick("bind_turretSpeed", 3, 1),
			inputMul("turretInputMul", 0.1), speed(0.f), frontInv("frontTurretMotorInveted", false),
			backInv("backTurretMotorInveted", false), turretUp("bind_turretUp", 3, 6),
			turretDown("bind_turretDown", 3, 8), angMotor("turret_angMotor", 5),
			angSpeed("turret_angSpeed", 0.1), pistonSolenoidExt("turret_pistonSolenoidExt", 1),
			pistonSolenoidRet("turret_pistonSolenoidRet", 2),
			fireButton("bind_turretFire", 3, 2), stopButton("bind_turretStop", 3, 11),
			presetButton("bind_turretPreset", 3, 1)
		{
		}
		
		virtual ~Turret()
		{
		}
		
		virtual void Init(Spyder::RunModes runmode)
		{
			Spyder::GetVictor(frontMotor.GetVal())->Set(0);
			Spyder::GetVictor(backMotor.GetVal())->Set(0);
			struct timespec tp;
			switch(runmode)
			{
				case Spyder::M_AUTO:
					autoPhase = 0;
					clock_gettime(CLOCK_REALTIME, &tp);
					autoStart = (double)tp.tv_sec + double(double(tp.tv_nsec)*1e-9);
					break;
				default:
					break;
			}
		}
		
		virtual void Periodic(Spyder::RunModes runmode)
		{
			switch(runmode)
			{
				case Spyder::M_TELEOP:
				{
					Joystick *joystick = Spyder::GetJoystick(turretJoystick.GetVal1());
					float val = joystick->GetRawAxis(turretJoystick.GetVal2()) * -1;
					val = fabs(val) > Spyder::GetDeadzone() ? val : 0;
					val *= inputMul.GetVal();
					speed += val;
					speed = (speed > 1.f) ? 1.f : speed;
					
					std::cout << "New turret speed: " << speed << std::endl;
					Spyder::Packet p;
					p.AddData<float>(speed);
					Spyder::Console::GetSingleton()->SendPacket("turr7et", p);
						
					
					if(Spyder::GetJoystick(presetButton.GetVal1())->GetRawButton(presetButton.GetVal2()))
					{
						speed = 0.371167;
					}
					
					if(Spyder::GetJoystick(stopButton.GetVal1())->GetRawButton(stopButton.GetVal2()))
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
					
					if(Spyder::GetJoystick(turretUp.GetVal1())->GetRawButton(turretUp.GetVal2()))
					{
						Spyder::GetVictor(angMotor.GetVal())->Set(angSpeed.GetVal());
					}
					else if(Spyder::GetJoystick(turretDown.GetVal1())->GetRawButton(turretDown.GetVal2()))
					{
						Spyder::GetVictor(angMotor.GetVal())->Set(angSpeed.GetVal() * -1);
					}
					else
					{
						Spyder::GetVictor(angMotor.GetVal())->Set(0);
					}
					
					if(Spyder::GetJoystick(fireButton.GetVal1())->GetRawButton(fireButton.GetVal2()))
					{
						Spyder::GetSolenoid(pistonSolenoidExt.GetVal())->Set(true);
						Spyder::GetSolenoid(pistonSolenoidRet.GetVal())->Set(false);
					}
					else
					{
						Spyder::GetSolenoid(pistonSolenoidExt.GetVal())->Set(false);
						Spyder::GetSolenoid(pistonSolenoidRet.GetVal())->Set(true);
					}
				}
					break;
				case Spyder::M_AUTO:
				{
					if(frontInv.GetVal())
					{
						Spyder::GetVictor(frontMotor.GetVal())->Set(-0.371167);
					}
					else
					{
						Spyder::GetVictor(frontMotor.GetVal())->Set(0.371167);
					}
					if(backInv.GetVal())
					{
						Spyder::GetVictor(backMotor.GetVal())->Set(-0.371167);
					}
					else
					{
						Spyder::GetVictor(backMotor.GetVal())->Set(0.371167);
					}
					struct timespec tp;
					clock_gettime(CLOCK_REALTIME, &tp);
					double curTime = (double)tp.tv_sec + double(double(tp.tv_nsec)*1e-9);
					double autoRunTime = curTime -  autoStart;
					switch(autoPhase)
					{
						case 0:
							if(autoRunTime >= 7)
							{
								++autoPhase;
								autoStart = curTime;
							}
							break;
						case 1:
							Spyder::GetSolenoid(pistonSolenoidExt.GetVal())->Set(true);
							Spyder::GetSolenoid(pistonSolenoidRet.GetVal())->Set(false);
							if(autoRunTime >= 0.7)
							{
								++autoPhase;
							}
							break;
						case 2:
							Spyder::GetSolenoid(pistonSolenoidExt.GetVal())->Set(false);
							Spyder::GetSolenoid(pistonSolenoidRet.GetVal())->Set(true);
							if(autoRunTime > 1.4)
							{
								autoPhase = 1;
								autoStart = curTime;
							}
							break;
					}
					break;
				}
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

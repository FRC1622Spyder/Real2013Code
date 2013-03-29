#include "Subsystem.h"
#include "WPIObjMgr.h"
#include "Config.h"
#include "Console.h"
#include <math.h>
#include <iostream>

class RollAvg
{
	private:
		std::vector<double> m_vals;
		size_t pos;
		size_t m_max;
	public:
		RollAvg(unsigned int n) : m_vals(n), pos(0), m_max(0)
		{
		}
		
		double Get()
		{
			double val = 0;
			for(size_t i = 0; i < m_max; ++i)
			{
				val += m_vals[i];
			}
			if(m_max == 0)
				return 0;
			return val/double(m_max);
		}
		
		void Add(double val)
		{
			size_t absMax = m_vals.size();
			if(pos > absMax)
			{
				pos = 0;
				m_max = absMax;
			}
			else if(m_max < absMax)
			{
				m_max = pos;
			}
			m_vals[pos++] = val;
		}
};

class Turret : public Spyder::Subsystem, public PIDOutput, public PIDSource
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
		
		Counter wheel1cntr;
		Counter wheel2cntr;
		timespec lastRead;
		PIDController pid;
		
		double m_rpm;
	public:
		Turret() : Spyder::Subsystem("Turret"), frontMotor("frontTurretMotor", 4),
			backMotor("backTurretMotor", 3), turretJoystick("bind_turretSpeed", 3, 1),
			inputMul("turretInputMul", 0.1), speed(0.f), frontInv("frontTurretMotorInveted", false),
			backInv("backTurretMotorInveted", false), turretUp("bind_turretUp", 3, 6),
			turretDown("bind_turretDown", 3, 8), angMotor("turret_angMotor", 5),
			angSpeed("turret_angSpeed", 0.1), pistonSolenoidExt("turret_pistonSolenoidExt", 1),
			pistonSolenoidRet("turret_pistonSolenoidRet", 2),
			fireButton("bind_turretFire", 3, 2), stopButton("bind_turretStop", 3, 11),
			presetButton("bind_turretPreset", 3, 1), wheel1cntr(4), wheel2cntr(5),
			pid(0, 0, 0, 0.3f, this, this, 0.05), m_rpm(0)
		{
			lastRead.tv_nsec = 0;
			lastRead.tv_sec = 0;
			wheel1cntr.Start();
			wheel2cntr.Start();
			pid.Disable();
			pid.SetOutputRange(-1, 1);
		}
		
		virtual ~Turret()
		{
		}
		
		virtual void PIDWrite(float output)
		{
			/*speed += output*0.01;
			speed = output;
			if(speed > 1.f)
			{
				speed = 1.f;
			}*/
		}
		
		virtual double PIDGet()
		{
			static RollAvg avg(10);
			timespec read;
			float time1 = lastRead.tv_sec;
			time1 += float(lastRead.tv_nsec)*1e-9;
			clock_gettime(CLOCK_REALTIME, &read);
			float time2 = read.tv_sec;
			time2 += float(read.tv_nsec)*1e-9;
			float time = time2-time1;
			if(time < 0.09f)
			{
				return m_rpm;
			}
			clock_gettime(CLOCK_REALTIME, &lastRead);
			double revs = wheel1cntr.Get()/3.0;
			wheel1cntr.Reset();
			double derp = 60.0/time;
			double my_rpm = revs;
			my_rpm *= derp;
			avg.Add(my_rpm);
			m_rpm = avg.Get();
			return m_rpm;
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
					if(!pid.IsEnabled())
						pid.Enable();
					Joystick *joystick = Spyder::GetJoystick(turretJoystick.GetVar(1));
					float val = joystick->GetRawAxis(turretJoystick.GetVar(2)) * -1;
					val = fabs(val) > Spyder::GetDeadzone() ? val : 0;
					//val *= inputMul.GetVal();
					double rpm = Spyder::GetJoystick(1)->GetRawAxis(3);
					rpm += 1;
					rpm /= 2.0;
					rpm *= 5000;
					pid.SetSetpoint(rpm);
					double p = Spyder::GetJoystick(2)->GetRawAxis(3);
					p += 1;
					p /= 2.0;
					p *= 0.001f;
					pid.SetPID(0.0005f, 0, 0);
					pid.Calculate();
					speed = p * pid.GetError();
					speed += 0.3;
					//speed += val;
					//speed = (speed > 1.f) ? 1.f : speed;
					//if(val != 0.0f)
					{
						std::cout << "New turret speed: " << speed << ", Setpoint: " << rpm << ", P: " << p
							<< "RPM: " << m_rpm << "Error: " << pid.GetError() << std::endl;
						Spyder::Packet p;
						p.AddData(speed);
						Spyder::Console::GetSingleton()->SendPacket("turr7et", p);	
					}
					
					if(Spyder::GetJoystick(presetButton.GetVar(1))->GetRawButton(presetButton.GetVar(2)))
					{
						speed = 0.371167;
					}
					
					if(Spyder::GetJoystick(stopButton.GetVar(1))->GetRawButton(stopButton.GetVar(2)))
					{
						speed = 0;
					}
					
					/*if(speed < 0.f)
					{
						speed = 0;
					}*/
					
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
				}
					break;
				case Spyder::M_AUTO:
				{
					if(!pid.IsEnabled())
						pid.Enable();
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
							}
							break;
						case 1:
							Spyder::GetSolenoid(pistonSolenoidExt.GetVal())->Set(true);
							Spyder::GetSolenoid(pistonSolenoidRet.GetVal())->Set(false);
							if(autoRunTime >= 7.2)
							{
								++autoPhase;
							}
							break;
						case 2:
							Spyder::GetSolenoid(pistonSolenoidExt.GetVal())->Set(false);
							Spyder::GetSolenoid(pistonSolenoidRet.GetVal())->Set(true);
							autoPhase = 0;
							autoStart = curTime;
							break;
					}
					break;
				}
				default:
					pid.Disable();
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

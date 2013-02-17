#include "Subsystem.h"
#include "WPIObjMgr.h"
#include "Config.h"

class Turret : public Spyder::Subsystem
{
	private:
		Spyder::ConfigVar<UINT32> frontMotor;
		Spyder::ConfigVar<UINT32> backMotor;
		
		Spyder::TwoIntConfig leftJoystick;
	public:
		Turret() : Spyder::Subsystem("Turret"), frontMotor("frontTurretMotor", 0),
			backMotor("backTurretMotor", 0), leftJoystick("bind_turretSpeed", 3, 1)
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
					Spyder::GetVictor(frontMotor.GetVal())->Set(0);
					Spyder::GetVictor(backMotor.GetVal())->Set(0);
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

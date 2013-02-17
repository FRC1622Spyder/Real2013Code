#include "Subsystem.h"
#include "WPIObjMgr.h"
#include "Config.h"

Spyder::ConfigVar<UINT32> frontMotor("frontTurretMotor", 0);
Spyder::ConfigVar<UINT32> backMotor("backTurretMotor", 0);

Spyder::TwoIntConfig leftJoystick("bind_turretSpeed", 3, 1);

class Turret : public Spyder::Subsystem
{
	public:
		Turret() : Spyder::Subsystem("Turret")
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

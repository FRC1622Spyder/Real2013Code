#include "Subsystem.h"
#include "WPIObjMgr.h"
#include "Config.h"


class Climb : public Spyder::Subsystem
{
	
	Spyder::ConfigVar<UINT32> ExtSol;
	Spyder::ConfigVar<UINT32> RetSol;
	Spyder::TwoIntConfig climbExt;
	Spyder::TwoIntConfig climbRet;
public:
	Climb() : Spyder::Subsystem("Climb"), ExtSol("climb_sol_ext",3), RetSol("climb_sol_ret", 4),
	climbExt("bind_climb_extend", 3, 3), climbRet("bind_climb_retract", 3, 4) //FIXME: get appropreate button numbers
	{
		
	}
	
	virtual ~Climb()
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
			Spyder::GetSolenoid(ExtSol.GetVal())->Set(false);
			Spyder::GetSolenoid(RetSol.GetVal())->Set(true);
			break;
		case Spyder::M_TELEOP:
			
			if(Spyder::GetJoystick(climbExt.GetVar(1))->GetRawButton(climbExt.GetVar(2))==true) {
				Spyder::GetSolenoid(ExtSol.GetVal())->Set(true);
				Spyder::GetSolenoid(RetSol.GetVal())->Set(false);
			}
			else if(Spyder::GetJoystick(climbRet.GetVar(1))->GetRawButton(climbRet.GetVar(2))==true) {
				Spyder::GetSolenoid(ExtSol.GetVal())->Set(false);
				Spyder::GetSolenoid(RetSol.GetVal())->Set(true);
			}
			break;
		default:
			Spyder::GetSolenoid(ExtSol.GetVal())->Set(false);
			Spyder::GetSolenoid(RetSol.GetVal())->Set(true);
		}
	}
	
	virtual void RobotInit()
	{
	}
};

Climb climb;

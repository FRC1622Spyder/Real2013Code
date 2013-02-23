#include "Subsystem.h"
#include "WPIObjMgr.h"
#include "Config.h"


class Pickup : public Spyder::Subsystem
{
	
	Spyder::ConfigVar<UINT32> ClampSol;
	Spyder::ConfigVar<UINT32> ReleaseSol;
	Spyder::TwoIntConfig cSol;
	Spyder::TwoIntConfig rSol;
	
	Spyder::ConfigVar<UINT32> arm;
	Spyder::TwoIntConfig stick;
public:
	Pickup() : Spyder::Subsystem("Pickup"), ExtSol("close_gripper_sol",5), RetSol("open_gripper_sol", 6),
	climbExt("bind_close_gripper", 3, 6), climbRet("bind_open_gripper", 3, 7), //FIXME: get appropreate button numbers
	arm("pickup_arm_mot", 12), stick("bind_pickup_arm_axis", 3, 2)
	{
		
	}
	
	virtual ~Pickup()
	{
		
	}
	virtual void Init(Spyder::RunModes runmode)
	{
		Spyder::GetSolenoid(ClampSol.GetVal())->Set(false);
		Spyder::GetSolenoid(ReleaseSol.GetVal())->Set(true);
		Spyder::GetVictor(arm.GetVal())->Set(0.0f);
	}
	
	virtual void Periodic(Spyder::RunModes runmode)
	{
		switch(runmode)
		{
		case Spyder::M_DISABLED:
			Spyder::GetSolenoid(ClampSol.GetVal())->Set(false);
			Spyder::GetSolenoid(ReleaseSol.GetVal())->Set(true);
			Spyder::GetVictor(arm.GetVal())->Set(0.0f);
			break;
		case Spyder::M_TELEOP:
			
			Spyder::GetVictor(arm.GetVal())->Set(Spyder::GetJoystick(stick.GetVar(1))->GetRawAxis(stick.GetVar(2)));
						
			if(Spyder::GetJoystick(climbExt.GetVar(1))->GetRawButton(climbExt.GetVar(2))==true) {
				Spyder::GetSolenoid(ClampSol.GetVal())->Set(true);
				Spyder::GetSolenoid(ReleaseSol.GetVal())->Set(false);
			}
			else if(Spyder::GetJoystick(climbRet.GetVar(1))->GetRawButton(climbRet.GetVar(2))==true) {
				Spyder::GetSolenoid(ClampSol.GetVal())->Set(false);
				Spyder::GetSolenoid(ReleaseSol.GetVal())->Set(true);
			}
			break;
		default:
			Spyder::GetSolenoid(ClampSol.GetVal())->Set(false);
			Spyder::GetSolenoid(ReleaseSol.GetVal())->Set(true);
			Spyder::GetVictor(arm.GetVal())->Set(0.0f);
		}
	}
	
	virtual void RobotInit()
	{
	}
};

Climb climb;

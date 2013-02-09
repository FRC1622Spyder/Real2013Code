#include <WPILib.h>
#include "Subsystem.h"


class RobotMain : public IterativeRobot
{
	private:
		unsigned int usPeriodCounter;
	public:
		virtual void StartCompetition()
		{
		}
		
		virtual void RobotInit()
		{
			this->SetPeriod(0.0);
			std::vector<Spyder::Subsystem*> subsystems = Spyder::SubsystemMgr::GetSingleton()->GetSubsystems();
			for(size_t i = 0; i < subsystems.size(); ++i)
			{
				subsystems[i]->RobotInit();
			}
		}
		
		virtual void DisabledInit()
		{
			std::vector<Spyder::Subsystem*> subsystems = Spyder::SubsystemMgr::GetSingleton()->GetSubsystems();
			for(size_t i = 0; i < subsystems.size(); ++i)
			{
				subsystems[i]->Init(Spyder::M_DISABLED);
			}
		}
		
		virtual void AutonomousInit()
		{
			std::vector<Spyder::Subsystem*> subsystems = Spyder::SubsystemMgr::GetSingleton()->GetSubsystems();
			for(size_t i = 0; i < subsystems.size(); ++i)
			{
				subsystems[i]->Init(Spyder::M_AUTO);
			}
		}
		
		virtual void TeleopInit()
		{
			std::vector<Spyder::Subsystem*> subsystems = Spyder::SubsystemMgr::GetSingleton()->GetSubsystems();
			for(size_t i = 0; i < subsystems.size(); ++i)
			{
				subsystems[i]->Init(Spyder::M_TELEOP);
			}
		}
		
		virtual void TestInit()
		{
			std::vector<Spyder::Subsystem*> subsystems = Spyder::SubsystemMgr::GetSingleton()->GetSubsystems();
			for(size_t i = 0; i < subsystems.size(); ++i)
			{
				subsystems[i]->Init(Spyder::M_TEST);
			}
		}
		
		virtual void DisabledPeriodic()
		{
			std::vector<Spyder::Subsystem*> subsystems = Spyder::SubsystemMgr::GetSingleton()->GetSubsystems();
			for(size_t i = 0; i < subsystems.size(); ++i)
			{
				if(usPeriodCounter % subsystems[i]->GetPeriod() == 0)
				{
					subsystems[i]->Periodic(Spyder::M_DISABLED);
				}
			}
			++usPeriodCounter;
		}
		
		virtual void AutonomousPeriodic()
		{
			std::vector<Spyder::Subsystem*> subsystems = Spyder::SubsystemMgr::GetSingleton()->GetSubsystems();
			for(size_t i = 0; i < subsystems.size(); ++i)
			{
				if(usPeriodCounter % subsystems[i]->GetPeriod() == 0)
				{
					subsystems[i]->Periodic(Spyder::M_AUTO);
				}
			}
			++usPeriodCounter;
		}
		
		virtual void TeleopPeriodic()
		{
			std::vector<Spyder::Subsystem*> subsystems = Spyder::SubsystemMgr::GetSingleton()->GetSubsystems();
			for(size_t i = 0; i < subsystems.size(); ++i)
			{
				if(usPeriodCounter % subsystems[i]->GetPeriod() == 0)
				{
					subsystems[i]->Periodic(Spyder::M_TELEOP);
				}
			}
			++usPeriodCounter;
		}
		
		virtual void TestPeriodic()
		{
			std::vector<Spyder::Subsystem*> subsystems = Spyder::SubsystemMgr::GetSingleton()->GetSubsystems();
			for(size_t i = 0; i < subsystems.size(); ++i)
			{
				if(usPeriodCounter % subsystems[i]->GetPeriod() == 0)
				{
					subsystems[i]->Periodic(Spyder::M_TEST);
				}
			}
			++usPeriodCounter;
		}
		
		virtual ~RobotMain()
		{
		}
		
		RobotMain()
		{
		}
};


START_ROBOT_CLASS(RobotMain);

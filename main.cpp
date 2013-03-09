#include "WPILib.h"
#include "Subsystem.h"
#include <fstream>
#include "Config.h"
#include "WPIObjMgr.h"
#include "Console.h"

Compressor *compr = NULL;

class RobotMain : public IterativeRobot
{
	private:
		unsigned int usPeriodCounter;
	public:
		virtual void RobotInit()
		{
			std::fstream file;
			file.open("config.cfg", std::ios_base::in);
			if(file.is_open())
			{
				Spyder::ConfigVarBase::ReadConfigFile(file);
				file.close();
			}
			
			this->SetPeriod(0.0);
			std::vector<Spyder::Subsystem*> subsystems = Spyder::SubsystemMgr::GetSingleton()->GetSubsystems();
			for(size_t i = 0; i < subsystems.size(); ++i)
			{
				subsystems[i]->RobotInit();
			}
			
			compr = new Compressor(2, 3);
			Spyder::Console *console = Spyder::Console::GetSingleton();
			console->Connect("10.16.22.5", 1140);
		}
		
		virtual void DisabledInit()
		{
			compr->Stop();
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
			if(Spyder::GetJoystick(1)->GetRawButton(9))
			{
				std::fstream file;
				file.open("config.cfg", std::ios_base::in);
				if(file.is_open())
				{
					Spyder::ConfigVarBase::ReadConfigFile(file);
					file.close();
				}
			}
			compr->Start();
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
		
		RobotMain() : IterativeRobot()
		{
		}
};


START_ROBOT_CLASS(RobotMain);

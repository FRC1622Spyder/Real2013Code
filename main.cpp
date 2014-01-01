#define PATH_MAX 255
#include "WPILib.h"
#include "Subsystem.h"
#include <fstream>
#include "Config.h"
#include "WPIObjMgr.h"
#include "Console.h"

//#define PROFILE

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
				#ifdef PROFILE
					timespec start;
					clock_gettime(CLOCK_REALTIME, &start);
				#endif
				subsystems[i]->RobotInit();
				#ifdef PROFILE
					timespec end;
					clock_gettime(CLOCK_REALTIME, &end);
					std::cout << "Subsystem " << subsystems[i]->GetName() << " took "
						<< end.tv_sec-start.tv_sec << "seconds and "
						<< end.tv_nsec-start.tv_nsec << "nanoseconds during robot init" << std::endl;
				#endif
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
				#ifdef PROFILE
					timespec start;
					clock_gettime(CLOCK_REALTIME, &start);
				#endif
				subsystems[i]->Init(Spyder::M_DISABLED);
				#ifdef PROFILE
					timespec end;
					clock_gettime(CLOCK_REALTIME, &end);
					std::cout << "Subsystem " << subsystems[i]->GetName() << " took "
						<< end.tv_sec-start.tv_sec << "seconds and "
						<< end.tv_nsec-start.tv_nsec << "nanoseconds during disabled init" << std::endl;
				#endif
			}
		}
		
		virtual void AutonomousInit()
		{
			std::vector<Spyder::Subsystem*> subsystems = Spyder::SubsystemMgr::GetSingleton()->GetSubsystems();
			for(size_t i = 0; i < subsystems.size(); ++i)
			{
				#ifdef PROFILE
					timespec start;
					clock_gettime(CLOCK_REALTIME, &start);
				#endif
				subsystems[i]->Init(Spyder::M_AUTO);
				#ifdef PROFILE
					timespec end;
					clock_gettime(CLOCK_REALTIME, &end);
					std::cout << "Subsystem " << subsystems[i]->GetName() << " took "
						<< end.tv_sec-start.tv_sec << "seconds and "
						<< end.tv_nsec-start.tv_nsec << "nanoseconds during auto init" << std::endl;
				#endif
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
				#ifdef PROFILE
					timespec start;
					clock_gettime(CLOCK_REALTIME, &start);
				#endif
				subsystems[i]->Init(Spyder::M_TELEOP);
				#ifdef PROFILE
					timespec end;
					clock_gettime(CLOCK_REALTIME, &end);
					std::cout << "Subsystem " << subsystems[i]->GetName() << " took "
						<< end.tv_sec-start.tv_sec << "seconds and "
						<< end.tv_nsec-start.tv_nsec << "nanoseconds during teleop init" << std::endl;
				#endif
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
					#ifdef PROFILE
						timespec start;
						clock_gettime(CLOCK_REALTIME, &start);
					#endif
					subsystems[i]->Periodic(Spyder::M_DISABLED);
					#ifdef PROFILE
						timespec end;
						clock_gettime(CLOCK_REALTIME, &end);
						std::cout << "Subsystem " << subsystems[i]->GetName() << " took "
							<< end.tv_sec-start.tv_sec << "seconds and "
							<< end.tv_nsec-start.tv_nsec << "nanoseconds during disabled periodic" << std::endl;
					#endif
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
					#ifdef PROFILE
						timespec start;
						clock_gettime(CLOCK_REALTIME, &start);
					#endif
					subsystems[i]->Periodic(Spyder::M_AUTO);
					#ifdef PROFILE
						timespec end;
						clock_gettime(CLOCK_REALTIME, &end);
						std::cout << "Subsystem " << subsystems[i]->GetName() << " took "
							<< end.tv_sec-start.tv_sec << "seconds and "
							<< end.tv_nsec-start.tv_nsec << "nanoseconds during auto periodic" << std::endl;
					#endif
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
					#ifdef PROFILE
						timespec start;
						clock_gettime(CLOCK_REALTIME, &start);
					#endif
					subsystems[i]->Periodic(Spyder::M_TELEOP);
					#ifdef PROFILE
						timespec end;
						clock_gettime(CLOCK_REALTIME, &end);
						std::cout << "Subsystem " << subsystems[i]->GetName() << " took "
							<< end.tv_sec-start.tv_sec << "seconds and "
							<< end.tv_nsec-start.tv_nsec << "nanoseconds during teleop periodic" << std::endl;
					#endif
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
					#ifdef PROFILE
						timespec start;
						clock_gettime(CLOCK_REALTIME, &start);
					#endif
					subsystems[i]->Periodic(Spyder::M_TEST);
					#ifdef PROFILE
						timespec end;
						clock_gettime(CLOCK_REALTIME, &end);
						std::cout << "Subsystem " << subsystems[i]->GetName() << " took "
							<< end.tv_sec-start.tv_sec << "seconds and "
							<< end.tv_nsec-start.tv_nsec << "nanoseconds during test periodic" << std::endl;
					#endif
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

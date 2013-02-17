#pragma once
#include <string>
#include <hash_map>
#include <vector>

namespace Spyder
{
	typedef enum {M_AUTO, M_DISABLED, M_TELEOP, M_TEST} RunModes;
	
	class Subsystem;
	
	class SubsystemMgr
	{
		public:
			static SubsystemMgr* GetSingleton();
			Subsystem* GetSubsystem(const std::string &strName);
			std::vector<Subsystem*>& GetSubsystems();
		private:
			friend class Subsystem;
			void RegisterSubsystem(const std::string &strName, Subsystem* ptr);
			std::vector<Subsystem*> m_subsystems;
			std::hash_map<std::string, Subsystem*> m_nameToSubsys; 
	};
	
	
	class Subsystem
	{
		public:
			Subsystem(const std::string &strName);
			virtual ~Subsystem() = 0;
			inline const std::string& GetName()
			{
				return m_strName;
			}
			virtual void Init(RunModes runmode) = 0;
			virtual void Periodic(RunModes runmode) = 0;
			virtual void RobotInit() = 0;
			inline unsigned short GetPeriod()
			{
				return m_usPeriod;
			}
			
			inline void SetPeriod(unsigned short usPeriod)
			{
				m_usPeriod = usPeriod;
			}
		private:
			std::string m_strName;
			unsigned short m_usPeriod;
	};
};

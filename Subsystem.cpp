#include "Subsystem.h"

namespace Spyder
{
	SubsystemMgr* SubsystemMgr::GetSingleton()
	{
		static SubsystemMgr* s_smgr = new SubsystemMgr;
		return s_smgr;
	}
	
	void SubsystemMgr::RegisterSubsystem(const std::string &strName, Subsystem* ptr)
	{
		m_subsystems.resize(m_subsystems.size() + 1);
		m_subsystems[m_subsystems.size()-1] = ptr;
		m_nameToSubsys[strName] = ptr;
	};
	
	std::vector<Subsystem*>& SubsystemMgr::GetSubsystems()
	{
		return m_subsystems;
	}
	
	Subsystem::Subsystem(const std::string &strName) : m_strName(strName), m_usPeriod(1)
	{
		SubsystemMgr::GetSingleton()->RegisterSubsystem(strName, this);
	}
	
	Subsystem::~Subsystem()
	{
	}
};

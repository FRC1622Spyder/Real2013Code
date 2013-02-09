#include "Config.h"
#include <hash_map>

std::hash_map<std::string, Spyder::ConfigVarBase*> cfgvar_map;

namespace Spyder
{
	void ConfigVarBase::ReadConfigFile(std::istream &file)
	{
		std::string strLine;
		while(!file.eof())
		{
			std::getline(file, strLine);
	
			size_t eqPos = strLine.find('=');
			std::string strName = strLine.substr(0, eqPos);
			std::string strVal = strLine.substr(eqPos+1, strLine.size()-eqPos-1);
			ConfigVarBase *var = cfgvar_map[strName.c_str()];
			if(var)
			{
				var->ReadVar(strVal);
			}
		}
	}
	
	template <class T>
	ConfigVar<T>::ConfigVar(std::string strName, T default_val): m_val(default_val)
	{
		cfgvar_map[strName] = this;
	}
	
	template <class T>
	void ConfigVar<T>::ReadVar(const std::string &dat)
	{
		std::stringstream ss(dat);
		ss >> m_val;
	}
}

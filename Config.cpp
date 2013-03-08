#include "Config.h"
#include <hash_map>
#include <sstream>
#include <iostream>
#include <ostream>
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
			if(eqPos == std::string::npos)
			{
				std::cout << "WARNING:CONFIG: couldnt find '=' token in: \n" << strLine << std::endl;
			}
			std::string strName = strLine.substr(0, eqPos);
			std::string strVal = strLine.substr(eqPos+1, strLine.size()-eqPos-1);
			ConfigVarBase *var = cfgvar_map[strName.c_str()];
			if(var)
			{
				var->ReadVar(strVal);
			}
		}
	}
	
	ConfigVarBase::ConfigVarBase(const std::string &strName) 
	{
		cfgvar_map[strName] = this;
	}
	
	TwoIntConfig::TwoIntConfig(const std::string &strName, int val1, int val2)
		: ConfigVarBase(strName), m_val1(val1), m_val2(val2)
	{
	}
	
	void TwoIntConfig::ReadVar(std::string &dat)
	{
		std::stringstream var1;
		std::stringstream var2;
		size_t pSemicolon = dat.find(';');
		if(pSemicolon == std::string::npos)
		{
			std::cout << "WARNING:CONFIG: couldnt find ';' token in: \n" << &dat <<std::endl;
		}
		var1 << dat.substr(0, pSemicolon);
		var2 << dat.substr(pSemicolon+1, dat.size()-pSemicolon-1);
		
		var1.seekg(std::ios_base::beg);
		var2.seekg(std::ios_base::beg);
		
		var1 >> m_val1;
		var2 >> m_val2;
	}
	
	int TwoIntConfig::GetVar(const unsigned char val)
	{
		switch(val)
		{
			case 1:
				return m_val1;
				break;
			case 2:
				return m_val2;
				break;
			default:
				return -1;
				break;
		}
	}
}

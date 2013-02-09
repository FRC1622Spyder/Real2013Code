#pragma once
#include <string>
#include <hash_map>
namespace Spyder
{
	class ConfigVarBase
	{
		protected:
			virtual void ReadVar(const std::string &dat) = 0;
		public:
			static void ReadConfigFile(std::istream &file);
	};

	std::hash_map<std::string, ConfigVarBase*> cfgvar_map;
	
	template <class T>
	class ConfigVar : ConfigVarBase
	{
		private:
			T m_val;
		public:
			ConfigVar(std::string strName, T default_val);
	
			inline T GetVal()
			{
				return m_val;
			}
		protected:
			virtual void ReadVar(const std::string &dat);
	};
};

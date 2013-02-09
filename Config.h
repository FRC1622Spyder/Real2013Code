#pragma once
#include <string>
namespace Spyder
{
	class ConfigVarBase
	{
		protected:
			virtual void ReadVar(const std::string &dat) = 0;
		public:
			static void ReadConfigFile(std::istream &file);
	};
	
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

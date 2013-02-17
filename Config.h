#pragma once
#include <string>
namespace Spyder
{
	class ConfigVarBase
	{
		protected:
			virtual void ReadVar(std::string &dat) = 0;
		public:
			static void ReadConfigFile(std::istream &file);
			ConfigVarBase(const std::string &strName);
	};
	
	template <class T>
	class ConfigVar : ConfigVarBase
	{
		private:
			T m_val;
		public:
			ConfigVar(const std::string &strName, T default_val);
	
			inline T GetVal()
			{
				return m_val;
			}
		protected:
			virtual void ReadVar(std::string &dat);
	};
	
	class TwoIntConfig : ConfigVarBase
	{
		private:
			int m_val1;
			int m_val2;
		public:
			TwoIntConfig::TwoIntConfig(const std::string &strName, int val1, int val2);
			int GetVar(const unsigned char val);
		protected:
			virtual void ReadVar(std::string &dat);
	};
};

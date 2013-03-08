#pragma once
#include <string>

namespace Spyder
{
	class Packet
	{
		private:
			std::string strData;
		public:
			Packet() : strData()
			{
			}
		
			template<class T>
			void AddData(T t)
			{
				strData += std::string((char*)(&t), sizeof(T));
			}
			
			void AddData(char* t)
			{
				unsigned int len = strlen(t);
				strData += std::string((char*)(&len), sizeof(unsigned int));
				strData += t;
			}
			
			void AddData(char *t, unsigned int length)
			{
				strData += std::string(t, length);
			}
	};
	
	class Console
	{
		public:
			Console();
			~Console();
			bool Connect(std::string strIP, unsigned short usPort);
			bool SendData(std::string strData);
	};
};

#pragma once
#define PATH_MAX 255
#include <string>
#include <sys/socket.h> //oh god, vxworks, why
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <vxWorks.h>
#include <stdioLib.h>
#include <sockLib.h>
#include <inetLib.h>
#include <strLib.h>
#include <hostLib.h>
#include <ioLib.h>
#include <sstream>
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
			
			void AddData(const char* t)
			{
				unsigned int len = strlen(t);
				strData += std::string((char*)(&len), sizeof(unsigned int));
				strData += t;
			}
			
			void AddData(char *t)
			{
				AddData((const char*)t);
			}
			
			template<class T>
			void AddData(T t)
			{
				strData += std::string((char*)(&t), sizeof(T));
			}
			
			void AddData(char *t, unsigned int length)
			{
				strData += std::string(t, length);
			}
			
			const std::string& GetData()
			{
				return strData;
			}
	};
	
	class Console
	{
		public:
			Console();
			~Console();
			static Console* GetSingleton();
			bool Connect(const std::string &strIP, unsigned short usPort);
			bool SendPacket(const std::string &strSubsystem, Packet &packet);
		private:
			int m_socket;
			struct sockaddr_in m_serverAddr;
	};
};

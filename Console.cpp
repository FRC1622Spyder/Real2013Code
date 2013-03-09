#include "Console.h"
#include <sstream>

bool Spyder::Console::Connect(const std::string &strIP, unsigned short usPort)
{
	m_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(m_socket == -1)
	{
		return false;
	}
	memset((void*)&m_serverAddr, 0, sizeof(m_serverAddr));
	m_serverAddr.sin_len = sizeof(sockaddr_in);
	m_serverAddr.sin_family = AF_INET;
	m_serverAddr.sin_port = usPort;
	m_serverAddr.sin_addr.s_addr = inet_addr(const_cast<char*>(strIP.c_str()));// I'm so sorry for this
	return true;
}

bool Spyder::Console::SendPacket(const std::string &strSubsystem, Packet& packet)
{
	std::stringstream ss;
	int length = strSubsystem.length();
	ss << std::string((char*)&length, 4);
	ss << strSubsystem;
	ss << packet.GetData();
	if(sendto(m_socket, const_cast<char*>(ss.str().c_str()), ss.str().length(), 0, (sockaddr*)&m_serverAddr, m_serverAddr.sin_len) == -1)
	{
		return false;
	}
	return true;
}

Spyder::Console* Spyder::Console::GetSingleton()
{
	static Spyder::Console *c = new Console;
	return c;
}

Spyder::Console::Console()
{
}
Spyder::Console::~Console()
{
}

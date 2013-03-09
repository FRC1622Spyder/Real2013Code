#include "Console.h"

bool Spyder::Console::Connect(std::string strIP, unsigned short usPort)
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

bool Spyder::Console::SendPacket(Packet& packet)
{
	if(sendto(m_socket, const_cast<char*>(packet.GetData().c_str()), packet.GetData().length(), 0, (sockaddr*)&m_serverAddr, m_serverAddr.sin_len) == -1)
	{
		return false;
	}
	return true;
}

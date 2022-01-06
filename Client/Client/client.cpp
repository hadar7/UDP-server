#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h> 
#include <string.h>

#define TIME_PORT	27015


void Send(int bytesSent, SOCKET connSocket, char sendBuff[], sockaddr_in server);
void Recieve(int bytesRecv, SOCKET connSocket, char recvBuff[]);
void MeasureRTT(int bytesSent, SOCKET connSocket, char sendBuff[], sockaddr_in server, char recvBuff[], int bytesRecv);
void helpForCase12(char cityName[], char sendBuff[], int bytesSent, SOCKET connSocket, sockaddr_in server, int bytesRecv, char recvBuff[]);
void GetClientToserverDelayEstimation(int bytesSent, SOCKET connSocket, char sendBuff[], sockaddr_in server, int bytesRecv, char recvBuff[]);

void main()
{

	// Initialize Winsock (Windows Sockets).

	WSAData wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Client: Error at WSAStartup()\n";
		return;
	}

	// Client side:
	// Create a socket and connect to an internet address.

	SOCKET connSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connSocket)
	{
		cout << "Time Client: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	// For a client to communicate on a network, it must connect to a server.

	// Need to assemble the required data for connection in sockaddr structure.

	// Create a sockaddr_in object called server. 
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(TIME_PORT);

	// Send and receive data.

	int bytesSent = 0;
	int bytesRecv = 0;
	char sendBuff[255];
	int sendQuesNum;
	char recvBuff[255];

	char cityName[20];

	bool stop = false;
	while (!stop)
	{
		cout << "choose question:" << endl;
		cout << "1.Get time " << endl;
		cout << "2.Get time without date " << endl;
		cout << "3.Get time since Epoch" << endl;
		cout << "4.Get client to server delay estimation" << endl;
		cout << "5. Measure RTT" << endl;
		cout << "6. Get time without date or seconds" << endl;
		cout << "7. Get year" << endl;
		cout << "8. Get month and day " << endl;
		cout << "9. Get seconds since begining of month" << endl;
		cout << "10. Get week of year " << endl;
		cout << "11. Get daylight savings" << endl;
		cout << "12. Get time without date in city" << endl;
		cout << "13. Measure time lap" << endl;
		cout << "14. Exit" << endl;

		cin >> sendQuesNum;
		_itoa(sendQuesNum, sendBuff, 10);

		if (sendQuesNum < 1 || sendQuesNum>14) // not in range
		{
			cout << "please choose number from menu" << endl;
		}
		else if (sendQuesNum == 12) // case 12 
		{
			helpForCase12(cityName ,sendBuff, bytesSent, connSocket, server, bytesRecv, recvBuff);
		}
		else if (sendQuesNum==4) // case 4
		{
			GetClientToserverDelayEstimation(bytesSent, connSocket, sendBuff, server, bytesRecv, recvBuff);
		}
		else if (sendQuesNum==5) // case 5
		{
			MeasureRTT(bytesSent, connSocket, sendBuff, server, recvBuff, bytesRecv);
		}
		else if (sendQuesNum==14)
		{
			stop = true;
		}
		else {
			Send(bytesSent, connSocket, sendBuff, server);
			Recieve(bytesRecv, connSocket, recvBuff);
		}	
	}
	// Closing connections and Winsock.
	cout << "Time Client: Closing Connection.\n";
	closesocket(connSocket);
	system("pause");
}
void Send(int bytesSent, SOCKET connSocket , char sendBuff [] , sockaddr_in server)
{
	bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&server, sizeof(server));
	if (SOCKET_ERROR == bytesSent)
	{
		cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
		closesocket(connSocket);
		WSACleanup();
		return;
	}
	cout << "Time Client: Sent: " << bytesSent << "/" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
}
void Recieve(int bytesRecv, SOCKET connSocket , char recvBuff [])
{
	bytesRecv = recv(connSocket, recvBuff, 255, 0);
	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
		closesocket(connSocket);
		WSACleanup();
		return;
	}

	recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
	cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" <<recvBuff << "\" message.\n";
}
void MeasureRTT(int bytesSent , SOCKET connSocket , char sendBuff[] ,sockaddr_in server , char recvBuff [] , int bytesRecv)
{
	double sum = 0;
	for (int i = 0; i < 100; i++)
	{
		bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&server, sizeof(server));
		DWORD  start = GetTickCount() ;
		if (SOCKET_ERROR == bytesSent)
		{
			cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
			closesocket(connSocket);
			WSACleanup();
			return;
		}
		cout << "Time Client: Sent: " << bytesSent << "/" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";

		bytesRecv = recv(connSocket, recvBuff, 255, 0);
		DWORD stop = GetTickCount();
		if (SOCKET_ERROR == bytesRecv)
		{
			cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
			closesocket(connSocket);
			WSACleanup();
			return;
		}
		sum += (stop - start);
	}
	sum = sum / 100;
	sum /= CLOCKS_PER_SEC;

	cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" << " the avarage RTT is :" << sum << "\" message.\n";
}
void GetClientToserverDelayEstimation(int bytesSent, SOCKET connSocket, char sendBuff[], sockaddr_in server, int bytesRecv, char recvBuff[])
{
	float res = 0;
	float lastTimer = 0;
	for (int i = 0; i < 100; i++)
	{
		Send(bytesSent, connSocket, sendBuff, server);
	}
	for (int i = 0; i < 100; i++)
	{
		bytesRecv = recv(connSocket, recvBuff, 255, 0);
		if (SOCKET_ERROR == bytesRecv)
		{
			cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
			closesocket(connSocket);
			WSACleanup();
			return;
		}
		recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string	
		double currTime = atof(recvBuff);
		if (lastTimer!=0)
		{
			res += (currTime - lastTimer);
		}
		lastTimer = currTime;
	}
	res /= 100;
	res /= CLOCKS_PER_SEC;
	cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" << " the avarage Delay Estimation is :" << res << "\" message.\n";
}
void helpForCase12( char cityName [],char sendBuff [] , int bytesSent,SOCKET connSocket , sockaddr_in server,int bytesRecv ,char recvBuff [])
{
	
	cout << "Enter city name : ( Tokyo, Melbourne ,San Francisco , Portugal )" << endl;
	cin >> cityName;
	strcat(sendBuff, cityName);
	Send(bytesSent, connSocket, sendBuff, server);
	Recieve(bytesRecv, connSocket, recvBuff);
}
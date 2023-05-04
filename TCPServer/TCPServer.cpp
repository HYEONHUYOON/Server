#include "Common.h"
#include <cstring>
#include <iostream>

#define SERVERPORT 9000
#define BUFSIZE    512

typedef struct {
	//flag Condition
	//0x00 => empty
	//0x01 => using
	//0x02 => full
	//0x03 => start

	char flag;
	char msg[1000];
}ringBuffer;

using namespace std;

static int ringBufferCount = 0;

int main(int argc, char *argv[])
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// ���� ����
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];
	
	ringBuffer msgBuffer[10];

	//STX Condition
	bool isSTX = false;

	char ContType[100];
	int ContLenth;

	char Body[1024];

	char coo[100];

	while (1) {

		// accept()
		addrlen = sizeof(clientaddr); 
		client_sock = accept(listen_sock, (struct sockaddr *)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// ������ Ŭ���̾�Ʈ ���� ���
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			addr, ntohs(clientaddr.sin_port));

		char cont[100];
		// Ŭ���̾�Ʈ�� ������ ���
		while (1) {
			isSTX = false;
			// ������ �ޱ�
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 0)
				break;
			
			string STX = "";

			//�����۷� ������ ����
			for (int i = 0, buffLength = 0; i < retval; i++){
		
				if (buf[i] == 'C')
				{
					int contCnt = 0;
					char contCheck = buf[i + contCnt];
					while (contCheck != ' ')
					{
						cont[contCnt] = contCheck;
						contCnt++;
						contCheck = buf[i + contCnt];
					}
					cont[contCnt] = '\0';

					strcpy(coo, cont);

					if (strcmp(cont, "Content-Length: ") == 0)
					{
						
					}
				    printf("%s", cont);
					//if(strcmp(contCheck))
				}
				//STX
				if (!isSTX) { 
					//������ Ȯ��
					for (int i = 0;i < 3; i++)
					{
						STX += buf[i];
					}
					if (STX.compare("GET") == 0 || STX.compare("PUT") == 0)
					{
						isSTX = true;
						msgBuffer[ringBufferCount].flag == 0x03;
					}
					else
					{
						STX += buf[3];
						if (STX.compare("POST") == 0)
						{
							isSTX = true;
							msgBuffer[ringBufferCount].flag == 0x03;
						}
						else {
							isSTX = false;
						}
					}
				}

				if (isSTX) {
					//���۰� ����� ��
					if (msgBuffer[ringBufferCount].flag == 0x00)
					{
						//���ۿ� �޼��� �ֱ�
						//���� ����
						msgBuffer[ringBufferCount].flag = 0x01;
						buffLength = 0;

					}
					//���ۿ� �����͸� �ִ� ���̶��
					else if (msgBuffer[ringBufferCount].flag == 0x01)
					{
						buffLength = strlen(msgBuffer[ringBufferCount].msg);
					}

					char ETX[4];

					if (buf[i] == '\r')
					{
						ETX[0] += buf[i];
						ETX[1] += buf[i + 1];
						ETX[2] += buf[i + 2];
						ETX[3] += buf[i + 3];
					}


					if (strcmp("\r\n\r\n",ETX) == 0) {
						msgBuffer[ringBufferCount].msg[buffLength] = '\n';
						msgBuffer[ringBufferCount].flag = 0x02;
						buffLength = 0;

						if (ringBufferCount > 8)
						{
							ringBufferCount = 0;
						}
						else
						{
							ringBufferCount++;
						}
					}
					else
					{
						msgBuffer[ringBufferCount].msg[buffLength++] = buf[i];
						msgBuffer[ringBufferCount].msg[buffLength] = '\0';
					}
				}
			}
			

			//STX �������� ������
			if (isSTX) {

				// ���� ������ ���
				buf[retval] = '\0';
				printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);

				printf("%s", coo);
			}
			else
			{
				cout << "STX ���ؿ� ���� ����" << endl;
			}

			// ������ ������
			retval = send(client_sock, buf, retval, 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}
		}

		// ���� �ݱ�
		closesocket(client_sock);
		printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			addr, ntohs(clientaddr.sin_port));
	}

	// ���� �ݱ�
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
}

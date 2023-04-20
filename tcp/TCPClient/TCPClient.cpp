#include "..\Common.h"
#include <stdio.h>

char *SERVERIP = (char *)"127.0.0.1";
#define SERVERPORT 9000
#define BUFSIZE    512
#define BUFSIZE2    100

int main(int argc, char *argv[])
{
	int retval;

	// ����� �μ��� ������ IP �ּҷ� ���
	if (argc > 1) SERVERIP = argv[1];

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// ���� ����
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	// ������ ��ſ� ����� ����
	char buf[BUFSIZE + 1];
	int len;

	char sendBuf[BUFSIZE2];

	// ������ ������ ���
	while (1) {
		// ������ �Է�
		printf("\n[���� ������] ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;


		//'\n' ���� ����
		len = (int)strlen(buf);
		if (buf[len - 1] == '\n')
			buf[len - 1] = 0x15;
		if (strlen(buf) == 0)
			break;

		strncpy(buf, "ABCD",sizeof(buf+1));
			
			// ������ ������ 

			int count = 0;
			for (int i = sizeof(sendBuf) * count; i < sizeof(sendBuf); i++)
			{
				sendBuf[i] = buf[i];
			}

			retval = send(sock, sendBuf, (int)strlen(sendBuf), 0);

			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}
			printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);
		

		//// ������ �ޱ�
		//retval = recv(sock, buf, retval, MSG_WAITALL);
		//if (retval == SOCKET_ERROR) {
		//	err_display("recv()");
		//	break;
		//}
		//else if (retval == 0)
		//	break;

		// ���� ������ ���
		buf[retval] = '\0';
		printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", retval);
		printf("[���� ������] %s\n", buf);
	}

	// ���� �ݱ�
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}

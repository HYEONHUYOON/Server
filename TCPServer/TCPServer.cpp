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

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 소켓 생성
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

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];
	
	ringBuffer msgBuffer[10];

	//STX Condition
	bool isSTX = false;

	while (1) {

		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr *)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			addr, ntohs(clientaddr.sin_port));

		// 클라이언트와 데이터 통신
		while (1) {

			isSTX = false;
			// 데이터 받기
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 0)
				break;
			
			string STX = "";

			//링버퍼로 데이터 관리
			for (int i = 0, buffLength = 0; i < retval; i++){

				//STX
				if (!isSTX) { 
					//세글자 확인
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
					//이 세개가 아니면 들여보내지 마셈
					//postman 으로 클라이언트 데이터 보내기
				
				}

				if (isSTX) {
					//버퍼가 비었을 때
					if (msgBuffer[ringBufferCount].flag == 0x00)
					{
						//버퍼에 메세지 넣기
						//상태 변경
						msgBuffer[ringBufferCount].flag = 0x01;
						buffLength = 0;

					}
					//버퍼에 데이터를 넣는 중이라면
					else if (msgBuffer[ringBufferCount].flag == 0x01)
					{
						buffLength = strlen(msgBuffer[ringBufferCount].msg);
					}

					string ETX = "";

					if (buf[i] == '\r')
					{
						ETX += buf[i];
						ETX += buf[i + 1];
						ETX += buf[i + 2];
						ETX += buf[i + 3];
					}

					if (ETX.compare("\r\n\r\n") == 0) {
						cout << ETX;
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
						ETX = "";
						msgBuffer[ringBufferCount].msg[buffLength++] = buf[i];
						msgBuffer[ringBufferCount].msg[buffLength] = '\0';
					}
				}
			}
			
			//STX 부합하지 않으면
			if (isSTX) {
				cout << STX << endl;
				// 받은 데이터 출력
				buf[retval] = '\0';
				printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);	
			}
			else
			{
				cout << "STX 기준에 맞지 않음" << endl;
			}

			// 데이터 보내기
			retval = send(client_sock, buf, retval, 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}
		}

		// 소켓 닫기
		closesocket(client_sock);
		printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
			addr, ntohs(clientaddr.sin_port));
	}

	// 소켓 닫기
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

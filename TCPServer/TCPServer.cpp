#include "Common.h"
#include <cstring>
#include <iostream>

#define SERVERPORT 9000
#define BUFSIZE    512

typedef __int8 Bool;

typedef struct {
	//flag Condition
	//0x00 => empty
	//0x01 => using
	//0x02 => full
	//0x03 => start

	char flag;
	char msg[1000];
}ringBuffer;

typedef struct {
	char method[10];
	char URL[100];
	char Version[100];
	char ContType[100];
	int ContLen;
}headerInform;

using namespace std;

static int ringBufferCount = 0;

Bool CheckMethod(char* method)
{
	if (!strcmp(method, "GET") || !strcmp(method, "POST") || !strcmp(method, "PUT"))
	{
		return 1;
	}
	return 0;
}


//헤더 내용 받기
Bool GetHederInformation(char* str,headerInform* receiveHeader) {
	char buf[BUFSIZ];
	buf[0]='\0';
	strcpy(buf, str);
	strcpy(receiveHeader->method,strtok(buf, " "));
	if (!CheckMethod(receiveHeader->method))
	{
		return 0;
	}
	strcpy(receiveHeader->URL, strtok(NULL, " "));
	strcpy(receiveHeader->Version, strtok(NULL, "\r\n"));
	
	char checkContent[BUFSIZ];
	while (1)
	{
		strcpy(checkContent, strtok(NULL, "\r\n"));

		//Body가 나오면 종료
		if (strcmp(checkContent, "{") == 0) {
			break;
		}

		char a[BUFSIZ];
		int i = 0;
		while (checkContent[i]!=':')
		{
			a[i] = checkContent[i];
			i++;
		}
		a[i] = '\0';

		if (strcmp(a, "Content-Type") == 0) {
			strcpy(receiveHeader->ContType, checkContent + i);
		}else if (strcmp(a, "Content-Length") == 0) {
			receiveHeader->ContLen = atoi(checkContent + (i+1));
		}
	}
	printf("METHOD : %s\nURL : %s\nVERSION : %s\nTYPE : %s\nLENGHT : %d\n", receiveHeader->method, receiveHeader->URL, receiveHeader->Version, receiveHeader->ContType, receiveHeader->ContLen);
	return 1;
}

//바디 받기
void GetBody(char* str,char* body)
{
	int i = 0;
	int bodyCnt = 0;
	while (str[i] != '\0')
	{
		//body
		if (str[i] == '{') {
			while (str[i] != '}') {
				body[bodyCnt] = str[i];
				i++;
				bodyCnt++;
			}
		}
		else {
			i++;
		}
	}
	body[bodyCnt] = '}';
	body[bodyCnt+1] = '\0';
}

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
	Bool isSTX = 0;

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

		// 접속한 클라이언트 정보 출력
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			addr, ntohs(clientaddr.sin_port));

		char cont[100];
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

			buf[retval] = '\0';

			string STX = "";
			
			headerInform recevieHeader;
			char body[BUFSIZE];
			if (!GetHederInformation(buf, &recevieHeader)) {
				isSTX = 1;
			}
			GetBody(buf,body);
			printf("BODY : %s\n", body);

			//링버퍼로 데이터 관리
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
				    //printf("%s", cont);
					//if(strcmp(contCheck))
				}
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
			

			//STX 부합하지 않으면
			if (isSTX) {

				buf[retval] = '\0';
				//printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);
				printf("%s\n",buf);

				//printf("%s", coo);
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

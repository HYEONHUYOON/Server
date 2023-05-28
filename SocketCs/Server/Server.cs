using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.IO;
using Newtonsoft.Json;

namespace Server
{
    class Server
    {
        const int BUFSIZE = 512;
        const int SERVERPORT = 8000;

        public int retval;
        Socket listen_sock = null;

        // 데이터 통신에 사용할 변수
        Socket client_sock = null;
        IPEndPoint clientaddr = null;
        byte[] buf = new byte[BUFSIZE];

        //로그 파일
        StreamWriter sw;

        bool isWhiteList = false;

        public Server()
        {
            try
            {
                sw = File.AppendText(@"..\..\ServerLog.txt");
                sw.WriteLine(DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss") + " => " + $"{"SERVER OPEN",-15}");
                sw.Flush();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                //Environment.Exit(1);
            }
            try
            {
                // 소켓 생성
                listen_sock = new Socket(AddressFamily.InterNetwork,
                    SocketType.Stream, ProtocolType.Tcp);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                //Environment.Exit(1);
            }
        }

        public void Bind()
        {
            try
            {
                // Bind()
                listen_sock.Bind(new IPEndPoint(IPAddress.Any, SERVERPORT));
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }

        public void Listen()
        {
            try
            {
                // Listen()
                listen_sock.Listen(Int32.MaxValue);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }

        public void Accept(List<string> ipList)
        {
            try
            {
                isWhiteList = false;

                // accept()
                client_sock = listen_sock.Accept();

                // 접속한 클라이언트 정보 출력
                clientaddr = (IPEndPoint)client_sock.RemoteEndPoint;

                sw.WriteLine(DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss") + " => "
                    + $"{"CONNECT TRY",-15} IP : {clientaddr.Address,-15} PORT : {clientaddr.Port,-10}");
                sw.Flush();

                Console.WriteLine(
                           "\n[TCP 서버] 클라이언트 접속시도: IP 주소={0}, 포트 번호={1}",
                           clientaddr.Address, clientaddr.Port);

                //ip리스트 검사
                for (int i = 0; i < ipList.Count(); i++)
                {
                    if (String.Compare(clientaddr.Address.ToString(), ipList[i]) == 0)
                    {
                        isWhiteList = true;
                    }
                }

                byte[] bytes;

                //접속 IP 검사
                if (!isWhiteList)
                {
                    Console.WriteLine("접속을 거부");
                    Console.Write("보낼 메세지 입력 : ");
                    string sendMsg = Console.ReadLine();

                    bytes = Encoding.Default.GetBytes(sendMsg);
                    client_sock.Send(bytes, bytes.Length, SocketFlags.None);

                    sw.WriteLine(DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss") + " => "
                    + $"{"SEND",-15} IP : {clientaddr.Address,-15} PORT : {clientaddr.Port,-10} {sendMsg}");
                    sw.Flush();

                    client_sock.Close();
                }
                else
                {
                    bytes = Encoding.Default.GetBytes("200");
                    client_sock.Send(bytes, bytes.Length, SocketFlags.None);

                    sw.WriteLine(DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss") + " => "
                    + $"{"SEND",-15} IP : {clientaddr.Address,-15} PORT : {clientaddr.Port,-10} {bytes}");
                    sw.Flush();
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }

        public void Read()
        {
            retval = 0;
            try
            {
                if (isWhiteList)
                {
                    // 데이터 받기
                    retval = client_sock.Receive(buf, 0, BUFSIZE, SocketFlags.None);
                    sw.WriteLine(DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss") + " => "
                       + $"{"RECEIVE",-15} IP : {clientaddr.Address,-15} PORT : {clientaddr.Port,-10} {Encoding.Default.GetString(buf)}");
                    sw.Flush();
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }

        }
        public void Write()
        {
            try
            {
                // 받은 데이터 출력
                Console.WriteLine("[TCP/{0}:{1}] {2}",
                    clientaddr.Address, clientaddr.Port,
                    Encoding.Default.GetString(buf, 0, retval));
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }

        }

        public void Send()
        {
            try
            {
                // 데이터 보내기
                client_sock.Send(buf, retval, SocketFlags.None);
                sw.WriteLine(DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss") + " => "
                 + $"{"SEND",-15} IP : {clientaddr.Address,-15} PORT : {clientaddr.Port,-10} {Encoding.Default.GetString(buf)}");
                sw.Flush();

                Console.WriteLine("[SEND] : {0}", Encoding.Default.GetString(buf, 0, retval));
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }

        }

        public void ClientSockClose()
        {
            try
            {
                // 소켓 닫기
                //client_sock.Close();

                Console.WriteLine(
                    "[TCP 서버] 클라이언트 종료: IP 주소={0}, 포트 번호={1}",
                    clientaddr.Address, clientaddr.Port);

                sw.WriteLine(DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss") + " => "
                + $"{"HOST CLOSE",-15} IP : {clientaddr.Address,-15} PORT : {clientaddr.Port,-10}");
                sw.Flush();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }

        }

        public void ServerClose()
        {
            try
            {
                sw.WriteLine(DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss") + " => " + $"{"SERVER CLOSE",-15}");
                sw.Close();
                // 소켓 닫기
                listen_sock.Close();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }

        }
    }
}

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
    class UDPServer
    {
        const int BUFSIZE = 512;
        const int SERVERPORT = 2469;

        public int retval;
        Socket listen_sock = null;

        // 데이터 통신에 사용할 변수
        IPEndPoint anyaddr = null;
        EndPoint peeraddr = null;
        byte[] buf = new byte[BUFSIZE];

        //로그 파일
        StreamWriter sw;

        bool isWhiteList = false;

        public UDPServer()
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
                // UDP 소켓 생성
                listen_sock = new Socket(AddressFamily.InterNetwork,
                    SocketType.Dgram, ProtocolType.Udp);
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

        public void ReceiveFrom(List<string> ipList)
        {
            retval = 0;
            try
            {
                anyaddr = new IPEndPoint(IPAddress.Any, 0);
                peeraddr = (EndPoint)anyaddr;

                //ip리스트 검사
                for (int i = 0; i < ipList.Count(); i++)
                {
                    if (String.Compare(anyaddr.Address.ToString(), ipList[i]) == 0)
                    {
                        isWhiteList = true;
                    }
                }

                // 데이터 받기
                retval = listen_sock.ReceiveFrom(buf, BUFSIZE,
                       SocketFlags.None, ref peeraddr);
                sw.WriteLine(DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss") + " => "
                   + $"{"RECEIVE",-15} IP : {anyaddr.Address,-15} PORT : {anyaddr.Port,-10} {Encoding.Default.GetString(buf)}");
                sw.Flush();
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
                    anyaddr.Address, anyaddr.Port,
                    Encoding.Default.GetString(buf, 0, retval));
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }

        }

        public void SendTo()
        {
            try
            {
                // 데이터 보내기
                listen_sock.SendTo(buf, 0, retval, SocketFlags.None, peeraddr);
                sw.WriteLine(DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss") + " => "
                 + $"{"SEND",-15} IP : {anyaddr.Address,-15} PORT : {anyaddr.Port,-10} {Encoding.Default.GetString(buf)}");
                sw.Flush();

                Console.WriteLine("[SEND] : {0}", Encoding.Default.GetString(buf, 0, retval));
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


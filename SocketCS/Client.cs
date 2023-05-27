﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;

namespace TCPClient
{

    class Client
    {
        public string SERVERIP;
        const int SERVERPORT = 8000;
        const int BUFSIZE = 512;

        int retval;

        // 데이터 통신에 사용할 변수
        byte[] buf;

        Socket sock;

        public Client() {
            try
            {
                // 소켓 생성
                sock = new Socket(AddressFamily.InterNetwork,
                    SocketType.Stream, ProtocolType.Tcp);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }

        // C/C++, Python과 달리 데이터 수신 소켓 함수가 MSG_WAITALL
        // 플래그를 지원하지 않으므로 해당 기능을 직접 구현한다.
        static int ReceiveN(Socket sock, byte[] buf, int len, SocketFlags flags)
        {
            int received;
            int offset = 0;
            int left = len;

            while (left > 0)
            {
                try
                {
                    received = sock.Receive(buf, offset, left, flags);
                    if (received == 0) break;
                    left -= received;
                    offset += received;
                }
                catch (Exception e)
                {
                    throw e;
                }
            }

            return len - left;
        }

        public bool Connect(string IP,string MSG)
        {
            try
            {
                Console.WriteLine(IP);
                var ipep = new IPEndPoint(IPAddress.Parse(IP), 8000);
                // Connect()
                sock.Connect(ipep);

                // 데이터 통신에 사용할 변수
                buf = new byte[BUFSIZE];

                // 데이터 보내기 (최대 길이를 BUFSIZE로 제한)
                byte[] senddata = Encoding.Default.GetBytes(MSG);
                int size = senddata.Length;

                if (size > BUFSIZE) size = BUFSIZE;
                retval = sock.Send(senddata, 0, size, SocketFlags.None);
                Console.WriteLine(
                    "[TCP 클라이언트] {0}바이트를 보냈습니다.", retval);

                //// 데이터 받기
                //retval = ReceiveN(sock, buf, 3, SocketFlags.None);
                //string msg = Encoding.Default.GetString(buf, 0, retval);
                //Console.WriteLine("[DENY MSG] : {0}", msg);

            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                return false;
            }
            return true;
        }

        public bool Write()
        {
            // 데이터 입력
            Console.Write("\n[보낼 데이터] ");
            string data = Console.ReadLine();
            if (data.Length == 0) return false;

            // 데이터 보내기 (최대 길이를 BUFSIZE로 제한)
            byte[] senddata = Encoding.Default.GetBytes(data);
            int size = senddata.Length;
            if (size > BUFSIZE) size = BUFSIZE;
            retval = sock.Send(senddata, 0, size, SocketFlags.None);
            Console.WriteLine(
                "[TCP 클라이언트] {0}바이트를 보냈습니다.", retval);

            return true;
        }

        public void Read()
        {
            // 데이터 받기
            retval = ReceiveN(sock, buf, retval, SocketFlags.None);
            //if (retval == 0) break;

            // 받은 데이터 출력
            Console.WriteLine(
                "[TCP 클라이언트] {0}바이트를 받았습니다.", retval);
            Console.WriteLine("[받은 데이터] {0}",
                Encoding.Default.GetString(buf, 0, retval));
        }

        public void Close()
        {
            try
            {
                // 소켓 닫기
                sock.Close();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                Environment.Exit(1);
            }
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;

namespace TCPClient
{
    class Program
    {
        static void Main(string[] args)
        {
            Client client;
            string IP;
            string MSG;

            bool connectStatus;

            while (true)
            {
                client = new Client();
                // 명령행 인수가 있으면 IP 주소로 사용
                if (args.Length > 0) client.SERVERIP = args[0];

                Console.WriteLine("0 => 프로그램 종료");

                Console.Write("IP 입력 : ");
                IP = Console.ReadLine();

                if(IP == "0")
                {
                    break;
                }

                Console.Write("MSG 입력 : ");
                MSG = Console.ReadLine();

                connectStatus = client.Connect(IP,MSG);

                Console.WriteLine("Enter 입력시 접속 종료");
                if (connectStatus)
                {
                    while (true)
                    {
                        bool isClose = client.Write();

                        //Enter시 Server가 죽음 버그 수정 바람
                        if(!isClose)
                        {
                            client.Close();
                            break;
                        }
                        client.Read();
                    }
                }
                else
                {
                    Console.WriteLine("접속 거부");
                }
            }
            client.Close();
        }
    }
}

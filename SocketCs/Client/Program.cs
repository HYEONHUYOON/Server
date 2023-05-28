using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;

namespace Client
{
    class Program
    {
        static void Main(string[] args)
        {
            Client client;
            CLIUI cli = new CLIUI();
            string IP = null;
            string MSG = null;

            bool connectStatus = false;

            int inputNum = 0;

            while (true)
            {
                client = new Client();
                // 명령행 인수가 있으면 IP 주소로 사용
                if (args.Length > 0) client.SERVERIP = args[0];

                //접속한적 있을 때
                if (connectStatus)
                {
                    inputNum = cli.QREpair();
                    if (inputNum == 1)
                    {
                        MSG = cli.QMSG();
                    }
                    else if (inputNum == 2)
                    {
                        IP = cli.QIP();
                    }
                }
                if (!connectStatus || inputNum == 0)
                {
                    Console.WriteLine("0 => 프로그램 종료");
                    IP = cli.QIP();
                    if (IP == "0")
                    {
                        break;
                    }
                    MSG = cli.QMSG();
                }

                connectStatus = client.Connect(IP, MSG);

                Console.WriteLine("Enter 입력시 접속 종료");
                if (connectStatus)
                {
                    while (true)
                    {
                        bool isClose = client.Write();
                        //Enter시 접속 종료
                        if (!isClose)
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

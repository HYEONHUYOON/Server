using System;
using System.Collections.Generic;

//using Internal;

namespace Server
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("TCP => 0");
            Console.WriteLine("UDP => 1");
            Console.Write("입력하시오 : ");
            int i = int.Parse(Console.ReadLine());

            JsonManager json = new JsonManager();
            json.LoadJson();
            List<string> IpList = json.GetIP();

            if (i == 0)
            {
                Console.WriteLine("TCP SERVER OPEN");
                TCPServer server = new TCPServer();

                server.Bind();
                server.Listen();
                while (true)
                {
                    try
                    {
                        server.Accept(IpList);
                        while (true)
                        {
                            server.Read();
                            //아무것도 오지 않으면 => 연결이 끊키면
                            if (server.retval == 0) break;
                            server.Write();
                            server.Send();
                        }

                        server.ClientSockClose();
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine(e.Message);
                        break;
                    }
                }
            }
            else
            {
                Console.WriteLine("UDP SERVER OPEN");
                UDPServer server = new UDPServer();

                server.Bind();

                while (true)
                {
                    try
                    {
                        while (true)
                        {
                            server.ReceiveFrom(IpList);
                            server.Write();
                            server.SendTo();
                        }
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine(e.Message);
                        break;
                    }


                }
            }
        }
    }
}

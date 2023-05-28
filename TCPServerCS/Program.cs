using System;
using System.Collections.Generic;

//using Internal;

namespace TCPServer
{
    class Program
    {
        static void Main(string[] args)
        {
            Server server = new Server();

            JsonManager json = new JsonManager();
            json.LoadJson();
            List<string> IpList = json.GetIP();

            server.Bind();
            server.Listen();
            while (true)
            {
                try
                {
                    server.Accept(IpList);
                    while(true)
                    {
                        server.Read();
                        //�ƹ��͵� ���� ������ => ������ ��Ű��
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

            //server.ServerClose();
        }
    }
}

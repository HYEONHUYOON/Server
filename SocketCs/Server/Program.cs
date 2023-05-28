using System;
using System.Collections.Generic;

//using Internal;

namespace Server
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

            //server.ServerClose();
        }
    }
}

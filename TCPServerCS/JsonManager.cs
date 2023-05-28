using System;
using System.Collections.Generic;
using System.IO;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace TCPServer
{
    class JsonManager
    {
        string json;
        JObject jobject;
        public JsonManager() {}
        
        public void LoadJson()
        {
            try
            {
                json = File.ReadAllText(@"..\..\clientList.json");
            }
            catch(Exception e)
            {
                Console.WriteLine(e.Message);
            }

            jobject = JObject.Parse(json);
        }
        public List<string> GetIP()
        {
            List<string> IpList = new List<string>();

            foreach (string s in jobject["IP"])
            {
                IpList.Add(s);
                Console.WriteLine(s);
            }

            return IpList;
        }
    }
}

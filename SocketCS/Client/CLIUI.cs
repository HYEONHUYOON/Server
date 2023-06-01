using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Client
{
    class CLIUI
    {
        public CLIUI() { }

        public string QIP()
        {
            string IP;

            Console.Write("IP 입력 :");
            IP = Console.ReadLine();

            return IP;
        }
        public string QMSG()
        {
            string MSG;

            Console.Write("MSG 입력 :");
            MSG = Console.ReadLine();

            return MSG;
        }
        public int QREpair()
        {
            Console.WriteLine("IP 와 MSG를 재사용 하십니까?");
            Console.WriteLine("0 : 재사용 안함");
            Console.WriteLine("1 : IP 재사용");
            Console.WriteLine("2 : MSG 재사용");
            Console.WriteLine("3 : 모두 재사용");
            int i = int.Parse(Console.ReadLine());

            return i;
        }
    }
}


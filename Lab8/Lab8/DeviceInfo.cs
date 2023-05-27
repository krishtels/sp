using System;
using System.Management;

namespace Lab8
{
    class DeviceInfo
    {
        public static string GetDeviceInfo(string queryString, string name)
        {
            var str = string.Empty;
            ManagementObjectSearcher searcher = new ManagementObjectSearcher($"select * from {queryString}");

            foreach (var i in searcher.Get())
            {
                var obj = (ManagementObject)i;

                foreach (var property in obj.Properties)
                {
                    if (property.Name == name)
                    {
                        str = property.Value.ToString();
                    }
                }
            }

            if (str == string.Empty)
            {
                return "Unknown";
            }

            return str;
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BS_Utils.Utilities;
using BeatSaberMarkupLanguage.Attributes;
using System.IO.Ports;

namespace SerialOutNew.Configuration
{
    class SerialConfig : PersistentSingleton<SerialConfig>
    {
        private Config config;
        [UIValue("boolEnable")]
        public bool modEnabled
        {
            get => config.GetBool("SerialOutNew", nameof(modEnabled), true, false);
            set => config.SetBool("SerialOutNew", nameof(modEnabled), value);
        }
        [UIValue("list-options")]
        private List<object> options = SerialPort.GetPortNames().ToList<object>();
        [UIValue("ComPort")]
        public string ComPort
        {
            get => config.GetString("SerialOutNew", nameof(ComPort), "");
            set => config.SetString("SerialOutNew", nameof(ComPort), value);
        }

        [UIValue("baud-options")]
        private List<object> rates = new object[] { 1200, 2400, 4800, 9600, 19200, 38400, 57600, 74880, 115200, 230400, 250000, 500000, 1000000 }.ToList();

        [UIValue("baud-choice")]
        public int baudChoice
        {
            get => config.GetInt("SerialOutNew", nameof(baudChoice), 9600);
            set => config.SetInt("SerialOutNew", nameof(baudChoice), value);
        }
        public void Awake()
        {
            config = new Config("SerialOutNew");
        }
    }
}

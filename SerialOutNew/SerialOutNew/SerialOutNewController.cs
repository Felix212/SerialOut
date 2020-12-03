using System;
using System.IO.Ports;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using SerialOutNew.Configuration;
using CustomJSONData;
using CustomJSONData.CustomBeatmap;
using SongCore.Utilities;

namespace SerialOutNew
{




    /// <summary>
    /// Monobehaviours (scripts) are added to GameObjects.
    /// For a full list of Messages a Monobehaviour can receive from the game, see https://docs.unity3d.com/ScriptReference/MonoBehaviour.html.
    /// </summary>
    public class SerialOutNewController : MonoBehaviour
    {
        private SerialPort port = new SerialPort(SerialConfig.instance.ComPort, SerialConfig.instance.baudChoice, Parity.None, 8, StopBits.One);
        private const int CHROMATYPE = 255;
        private const int COLOR_ONLY_VALUE = 10;
        public static SerialOutNewController instance { get; private set; }
        private BeatmapObjectCallbackController Ec;
        internal const int RGB_INT_OFFSET = 2000000000;
        List<int> _lightTypes = new List<int> { 0, 1, 2, 3, 4, 12, 13 };
        #region Monobehaviour Messages
        /// <summary>
        /// Only ever called once, mainly used to initialize variables.
        /// </summary>
        private void Awake()
        {
            Logger.log?.Debug($"{name}: Awake()");

            port.Open();
            Logger.log.Info("Open Port");
            Ec = Resources.FindObjectsOfTypeAll<BeatmapObjectCallbackController>().FirstOrDefault();
            
            StartCoroutine(GrabLight());
        }
        IEnumerator GrabLight()
        {
            yield return new WaitUntil(() => Resources.FindObjectsOfTypeAll<BeatmapObjectCallbackController>().Any());
            Ec = Resources.FindObjectsOfTypeAll<BeatmapObjectCallbackController>().FirstOrDefault();
            Logger.log.Info("Found LightController");
            Ec.beatmapEventDidTriggerEvent += EventHappened;
            var beatmap = SongCore.Collections.RetrieveDifficultyData(BS_Utils.Plugin.LevelData.GameplayCoreSceneSetupData.difficultyBeatmap)?.additionalDifficultyData;
            BeatmapData _beatmapData = Ec?.GetField<BeatmapData>("_beatmapData");

            if (beatmap == null)
            {
                Logger.log.Info("Disable Chroma");
                port.Write(new byte[] { (byte)CHROMATYPE, (byte)0 }, 0, 2);
            }
            else
            if (beatmap._requirements.Contains("Chroma") || beatmap._suggestions.Contains("Chroma") || (_beatmapData?.beatmapEventsData?.Any(n => n.value >= 2000000000) ?? false))
            {
                //Enable chroma
                Logger.log.Info("enable Chroma");
                port.Write(new byte[] { (byte)CHROMATYPE, (byte)1 }, 0, 2);
            } else 
            {
                Logger.log.Info("Disable Chroma");
                port.Write(new byte[] { (byte)CHROMATYPE, (byte)0 }, 0, 2);
            }
        }
        /// <summary>
        /// Only ever called once on the first frame the script is Enabled. Start is called after any other script's Awake() and before Update().
        /// </summary>
        private void Start()
        {
            Logger.log?.Debug($"{name}: Start()");
            
        }

        /// <summary>
        /// Called every frame if the script is enabled.
        /// </summary>
        private void Update()
        {

        }

        /// <summary>
        /// Called every frame after every other enabled script's Update().
        /// </summary>
        private void LateUpdate()
        {

        }

        /// <summary>
        /// Called when the script becomes enabled and active
        /// </summary>
        private void OnEnable()
        {
            Logger.log.Info("Enable");

        }

        /// <summary>
        /// Called when the script becomes disabled or when it is being destroyed.
        /// </summary>
        private void OnDisable()
        {
            Logger.log.Info("Port closed");
            port.Close();
        }

        /// <summary>
        /// Called when the script is being destroyed.
        /// </summary>
        private void OnDestroy()
        {
            Logger.log?.Debug($"{name}: OnDestroy()");
            instance = null; // This MonoBehaviour is being destroyed, so set the static instance property to null.
            port.Close();
        }
        #endregion
        void EventHappened(BeatmapEventData Data)
        {
            //Logger.log.Info(Data.type.ToString());
            int Event;
            int value = Data.value;
            int.TryParse(Data.type.ToString().Replace("Event", ""), out Event);
            int light;
            //legacy chroma events save rgb in value attribute
            if (value > 2000000)
            {
                light = (Event << 4) + COLOR_ONLY_VALUE; //set value to something unused. Controller will only set light
            }
            else
            {
                //save event + value as one byte
                light = (Event << 4) + value;
            }
            if (_lightTypes.Contains(Event))
            {
                // color 0 is no color
                byte color = 0;
                if (Data is CustomBeatmapEventData)
                {
                    CustomBeatmapEventData customData = Data as CustomBeatmapEventData;
                    dynamic data = customData.customData;
                    Color? colorData = GetColorFromData(data);
                    if (colorData != null || value > 2000000)
                    {
                        //legacy chroma
                        if (value > 2000000)
                        {
                            colorData  = ColorFromInt(value);
                        }
                        Color32? To256Base = colorData;
                        Logger.log.Debug(To256Base.ToString());
                        int onebytecolor = ((To256Base.Value.r / 32) << 5) + ((To256Base.Value.g / 32) << 2) + (To256Base.Value.b / 64);
                        color = (byte)onebytecolor;
                    }
                    Logger.log.Debug(Event.ToString() + " // " + value.ToString() + " // " + color.ToString());
                }
                port.Write(new byte[] {(byte)light, (byte) color}, 0, 2); 
            }
        }



        //helper fuctions. Thanks to Aeroluna
        //extract color from _color attribute 
        internal Color? GetColorFromData(dynamic data, string member = "_color")
        {
            IEnumerable<float> color = ((List<object>)Trees.at(data, member))?.Select(n => Convert.ToSingle(n));
            if (color == null)
            {
                return null;
            }

            return new Color(color.ElementAt(0), color.ElementAt(1), color.ElementAt(2), color.Count() > 3 ? color.ElementAt(3) : 1);
        }
        //convert legacy chroma events
        internal Color ColorFromInt(int rgb)
        {
            rgb -= RGB_INT_OFFSET;
            int red = (rgb >> 16) & 0x0ff;
            int green = (rgb >> 8) & 0x0ff;
            int blue = rgb & 0x0ff;
            return new Color(red / 255f, green / 255f, blue / 255f);
        }
    }
}

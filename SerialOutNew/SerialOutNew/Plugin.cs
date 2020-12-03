using IPA;
using UnityEngine;
using UnityEngine.SceneManagement;
using IPALogger = IPA.Logging.Logger;
using SerialOutNew.Configuration;
using BeatSaberMarkupLanguage.Settings;

namespace SerialOutNew
{
    [Plugin(RuntimeOptions.SingleStartInit)]
    public class Plugin
    {
        // TODO: If using Harmony, uncomment and change YourGitHub to the name of your GitHub account, or use the form "com.company.project.product"
        //       You must also add a reference to the Harmony assembly in the Libs folder.
        // public const string HarmonyId = "com.github.YourGitHub.SerialOutNew";
        // internal static HarmonyInstance harmony => HarmonyInstance.Create(HarmonyId);

        internal static Plugin instance { get; private set; }
        internal static string Name => "SerialOutNew";
        internal static SerialOutNewController PluginController { get { return SerialOutNewController.instance; } }
        internal static PluginConfig config;
        public bool eventAdded = false;

        [Init]
        public void Init(IPALogger logger)
        {
            Logger.log = logger;
        }
        [OnStart]
        public void OnStart()
        {
            BSMLSettings.instance.AddSettingsMenu("SerialOut", "SerialOutNew.Configuration.Settings.bsml", SerialConfig.instance);
            SceneManager.activeSceneChanged += SceneManagerOnActiveSceneChanged;
            //BSEvents.menuSceneLoaded += BSEvents_gameSceneLoaded;
        }

        #region Disableable

        /// <summary>
        /// Called when the plugin is enabled (including when the game starts if the plugin is enabled).
        /// </summary>
        [OnEnable]
        public void OnEnable()
        {
            //ApplyHarmonyPatches();
        }
        private void SceneManagerOnActiveSceneChanged(Scene oldScene, Scene newScene)
        {
            if (newScene.name == "MenuViewControllers" && oldScene.name == "EmptyTransition")
            {
            }
            if (newScene.name == "GameCore" && SerialConfig.instance.modEnabled)
            {
                new GameObject("SerialOutNewController").AddComponent<SerialOutNewController>();

            }


        }
        [OnDisable]
        public void OnDisable()
        {
            if (PluginController != null)
                GameObject.Destroy(PluginController);
            //RemoveHarmonyPatches();
        }
    }
}
#endregion
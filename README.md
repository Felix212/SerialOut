# SerialOut
Beat saber mod that sends light events to serialport. Events can be used by a controller (ESP32, Arduino). Chroma is also supported.

## Requirements:
	BSIPA
	CustomJSONData
	BeatSaberMarkupLanguage
	BS Utils

You can get all dependencies from ModAssistant. The controller code is written for an ESP32 combined with an [WS2811](https://www.amazon.de/gp/product/B01CNL6K52/ref=ppx_yo_dt_b_asin_title_o03_s00?ie=UTF8&psc=1)  LED stripe. I recommend the [platformio](https://platformio.org/) extentions for vscode to compile the code. Normal lightmaps should run at 9600 baudrate. Intense lightmaps with a lot of custom lights can cause lag if the baudrate is too low. The maximum I have test is 250000. 

You can also write your own light program.

Every lightevent is 2 bytes long. First byte contains light type and light action. First 4 bits are light action, last 4 bits are light type. Second byte contains rgb data. I compressed the data to 2 byte, because intense lightmaps caused an UART bufferoverflow.

[Preview](https://www.youtube.com/watch?v=0ZJlF3XPXzM)

## Special thanks to:



[D3rPole](https://github.com/D3rPole) [MyLegIsPotato](https://github.com/MyLegIsPotato) for idea and for old code

[Aeroluna](https://github.com/Aeroluna) for Chroma code


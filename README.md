<p align="center"><img alt="MudPi Smart Garden" title="MudPi Smart Garden" src="https://mudpi.app/img/mudPI_LOGO_small_grad.png" width="200px"></p>

# MudPi Passive Node
> ESP32 firmware to take sensor readings, monitor battery levels and send data wirelessly to MudPi. The unit is typically powered off a battery with solar charging and spends most of the time in deep sleep.


## Installation
Download the repository and place it inside your `Arduino` folder. 

Open the Arduino IDE and select `passive-node`. 

Navigate to the `config.h` file and make your desired configuration changes such as adding your WiFi credentials and setting the MQTT server. 

Flash firmware to the device (ESP32 or ESP8266)

Advanced options can be found in `system.h`

## How It Works
The node will wake up from deep sleep and cycle through the sensors you set in your configuration. It will take a reading of each of the sensors and then attempt to send this data over WiFi as JSON to the `server_url` or over MQTT to the `server_mqtt` that you set in the `config.h` file. 
The data will arrive in a JSON format like the one below:
```
{
  "source": 4 // Device ID 
  "boot": 17 // The current boot count
  "value": [ // array of sensor readings (see below)
    {
      "value": 1300, // raw value of the sensor read
      "sensor": "soil", // sensor that took the reading
      "parsed": 70 // value turned into percent
      "source": "33" // The pin the reading was taken on
      "boot": 17 // boot count the reading was taken on
    }
  ] 
}
```

## Sending Data over MQTT
Sending data can be done to either an http endpoint or to a MQTT broker. If you set a `server_mqtt` inside the `config.h` file then the node will attempt to post the data over MQTT. It will try to post the data to the topic set in the `mqtt_topic`. It is recommended to only use one data posting method either HTTP or MQTT. If you wish to send data into MudPi then you should use the MQTT method and configure MQTT sensor in MudPi for each of the expected readings.

### Grouping Send Queries
In the `system.h` file you may set the `SEND_DATA_GROUPED` option to `0` if you want each reading to be sent individually. By default this option is set to `1`. With group data sending enabled, the node will take all the reading and pack them in a `data` array to send in one request. This can be useful to cutdown data requests and have faster operations but then requires more data parsing on the server end. 

If you disable the group data sending then the node will send data to a topic determined by a combintaion of the option set in `config.h` and the sensor data. For example if `mqtt_topic` was set as `mudpi/node/1` then a `soil` sensor on pin `33` would send its data to `mudpi/node/1/soil/33`. The format for the generated topic is `{mqtt_topic}/{sensor_typ}/{pin}`

## Sending Data over HTTP
You are expected to provide your own HTTP endpoint in order to store the readings sent by the node. The endpoint can be local or a remote address. The node will send the data to the endpoint and wait for a `200` response. The endpoint you use should be able to receive the JSON object in the format shown above. You can set the endpoint by changing the `server_url` inside the `config.h` file from `NULL`.

## Lower Power Consumption
The main design of this firmware is to run on a battery powered device with minimal power consumption. The device is set to go to deep sleep after sending a batch of sensor readings. In the `config.h` you can change the `TIME_TO_SLEEP` to alter the duration of the deep sleep in seconds. 

Another way the firmware lowers power use is by controlling when sensors are powered and turning them on only when they are needed to take a reading. This is done through the use of another GPIO pin. You connect the sensor PWR to the desired GPIO pin which will then be toggled on just berfore its time to take a sensor reading. This pin can be configured in the `config.h` file under the `sensor_enable_pin` option. When in deep sleep sensors will then be turned off saving more power and extending battery life.

## Fail Protection
The node will attempt to send up to 10 previous failed readings if a failure occurs during sending. The failed readings will attempt to be sent BEFORE the new reading from the current boot cycle. The readings are stored in RTC memory and will persist over deep sleep. Once the device wakes up it checks for failed readings and will handle those first. Keep in mind if the device crashes or restarts otherwise these readings will be lost as they are in memory.

## Passive vs Active Nodes
A passive node is one that is designed to handle its own logic and configuration. This node will perform its operation independently and then send it to the main controller. This allows for the passive node to be in deep sleep or perform other complex operations and then notify MudPi of the results. This is different from an active node which awaits for its instruction and commands from the main controller. This requires an 'always on' connection so that it does not miss a command. An always on connection can consume more power and is harder to use on battery. 

The other main consideration when choosing passive vs active nodes is the role the node fulfills. A active node fits better in mission critical components that requires a more consistent communication between the controller and node. Where a passive node is better suited to run components that it may be ok to miss a reading here and there. For example its ok if I miss one soil reading out of 30 sent every hour. Where its not ok if I miss a signal to turn off a pump ever. Systems can be comprised of both passive and active nodes to solve a variety of setup needs.

## Early Version
Any contributions you can make will be greatly appreciated. This is early version firmware and not guaranteed to work for every device or situation. I am only a solo developer balancing many projects so updates come as time and funding permits. This firmware is subject to breaking changes.


## Versioning
Breaking.Major.Minor

*Works with MudPi Core v0.10.0+*

## Authors
* Eric Davisson  - [Website](http://ericdavisson.com)
* [Twitter.com/theDavisson](https://twitter.com/theDavisson)

## Community
* Discord  - [Join](https://discord.gg/daWg2YH)
* [Twitter.com/MudpiApp](https://twitter.com/mudpiapp)
* Support Me  - [Patreon](https://patreon.com/mudpi)


## Documentation
*Please note this firmware is in early stages and full documentation is not available.*

For general documentation visit [mudpi.app](https://mudpi.app/docs)


## Hardware Tested On
* ESP32
* ESP8266 (swap the wifi library in `NetworkController`)

## Sensors Supported
There are several sensors available to use. A sensor hooks up to a pin and takes a reading. Depending on the sensor type it can process that data in different ways. 

### Soil Sensor
The soil sensor takes an analog reading on the configured pin. The soil sensor will take a sample of 5 readings and return the average to reduce the noise from readings.

### Voltage Sensor (3.7v Battery)
A voltage sensor is used to measure voltages, particularly with batteries. The voltage sensor will use the ESP internal ADC to take an analog reading on the set pin. For more accurate readings the analog resolution is set to 11 bits and the attenuation is set to 0db. This reduces the voltage range of the ADC pin but increases the reading accuracy. (1V input = ADC reading of 1088).

In order to read a 3.7v battery with these settings a voltage divider is required to bring the readings within range. For my own builds I use two resisters of values 100k and 22k to bring the 3.7v down to around \~.75v. This allows for the battery to be read with more accurate results. You can change the resistor settings in `system.h`.

### Light Intensity Sensor
The light sensor takes an analog reading on the configured pin. The light sensor will take a sample of 5 readings and return the average to reduce the noise from readings. This is typically light intensity and not actual lux.

### DHT Sensor
The DHT sensors are not recommended on battery and prone to errors regardless. You can still enable a DHT sensor with this firmware but only 1 is allow to be connected. This is because the DHT library is not developer friendly and hard to extrapolate. You can find the `DHT_PIN` setting in your `config.h` file. The default for this is `NULL` to disable the DHT functionality. 

### Sensors still to be added
- [x] DHT 11 / 22

- [ ] Pressure / Barometer

- [x] Light

- [ ] Onewire Temperature (DS18B20)

- [ ] Float Sensor (could just use soil or light for now)


## Example Sensor Config
This config below is an example of how your `config.h` file would look with two soil sensors (on pins 32 & 33) and one voltage sensor (on pin 36).
```
#define TOTAL_SENSORS_SOIL  2
byte soil_sensor_pins[TOTAL_SENSORS_SOIL] = {32, 33};

#define TOTAL_SENSORS_VOLTAGE  1
byte voltage_sensor_pins[TOTAL_SENSORS_VOLTAGE] = {36};

char* server_mqtt = "192.168.2.64";
char* mqtt_topic = "mudpi/node/1";
```

Remember if `SEND_DATA_GROUPED` is disabled then each sensor will broadcast on its own topic. With the following configuration above the sensor would send on the following topics:
```
mudpi/node/1/soil/32
mudpi/node/1/soil/33
mudpi/node/1/voltage/36
```

Here is an example of a `mudpi.config` entry to listen for these sensor readings through the MQTT extension.
```
...
"sensor": [
  {
      "name": "Sprout 7 Soil 1",
      "interface": "mqtt",
      "connection": "local_mqtt",
      "key": "sprout_7_soil_1",
      "topic": "mudpi/sprout/7/soil/32",
      "expires": 0
  },
  {
      "name": "Sprout 7 Soil 2",
      "interface": "mqtt",
      "connection": "local_mqtt",
      "key": "sprout_7_soil_2",
      "topic": "mudpi/sprout/7/soil/33",
      "expires": 0
  },
  {
      "name": "Sprout 7 Battery",
      "interface": "mqtt",
      "connection": "local_mqtt",
      "key": "sprout_7_battery",
      "topic": "mudpi/sprout/7/voltage/36",
      "expires": 0
  }]
  "mqtt": {
      "key": "local_mqtt",
      "host": "localhost",
      "port": 1883
  }
```

## Keep Power Lower to Avoid Brownouts / Crashes
With the ESP32 device running on a 3.7v battery can be done with well if you take a couple extra steps in your projects. These extra steps will help ensure lower power consumption and help reduce power spikes that could crash or brownout your device. ESP devices are not perfect and do crash. It happens. Below are some tips I use on my battery ESP32 devices to reduce crashes and extend life in the field:

- Have a capacitor (\~ 10uF) close to ESP32 PWR)
- Keep sensors on toggled power source to prevent vampire drain
- Turn on sensors only for readings
- Turn sensors off after a reading
- Try to avoid lots of Wifi activity
- Turn off sensors before any Wifi use
- Use a reliable power converter of 500mA or more
- Pay attention to pins that pull LOW/HIGH on boot/sleep

## License
If you use this project share it online or send me a postcard! :)

## Keep Growing
Nothing great comes without work. If you run into errors keep on trying! I personally spent hundreds of hours on debugging these devices tweaking small changes. Even still crashes and errors happen, electronics are not perfect. 

<img alt="MudPi Smart Garden" title="MudPi Smart Garden" src="https://mudpi.app/img/mudPI_LOGO_small_flat.png" width="50px">


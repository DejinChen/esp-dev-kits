# Smart Panel With Matter + Rainmaker Controller Example

A matter + rainmaker controller demo based on [ESP32-S3-LCD-EV-Board](https://www.espressif.com/en/products/devkits) development board with subboard3 (800x480).

## Prerequisites
- [ESP-IDF](https://github.com/espressif/esp-idf)
- [ESP-Matter SDK](https://github.com/espressif/esp-matter)
- [ESP Rainmaker SDK](https://github.com/espressif/esp-rainmaker)
- [ESP Secure Cert Manager](https://github.com/espressif/esp_secure_cert_mgr)

Please go through the installation process (if required) for all of the above prerequisites.

## Setting up the environment
For building, IDF, ESP-Matter environment, and RainMaker path are required.

```
$ cd /path/to/esp-idf
$ . ./export.sh
$ cd /path/to/esp-matter
$ . ./export.sh
$ export RMAKER_PATH=/path/to/esp-rainmaker
```

### Claiming device certificates

Self Claiming or Assisted Claiming can't be used with the RainMaker + Matter examples because the certificate needs to be present even before Matter commissioning stars.
So, we will use [host driven claiming](https://rainmaker.espressif.com/docs/claiming#host-driven-claiming) via the RainMaker CLI.

Make sure your device is connected to the host machine, login and claim:

```
$ cd $RMAKER_PATH/cli
$ ./rainmaker.py login
$ ./rainmaker.py claim --matter <port>
```

This will fetch the device certificates and flash them on your device.

### Generating the factory nvs binary

The factory nvs (fctry partition) needs to be generated using the mfg_tool of esp-matter

```
$ export ESP_SECURE_CERT_PATH=/path/to/esp_secure_cert_mgr
$ cd $ESP_MATTER_PATH/tools/mfg_tool
$ ./mfg_tool.py -v 0x131B -p 0x2 --passcode 125 --discriminator 521 -cd $RMAKER_PATH/examples/matter/mfg/cd_131B_0002.der --csv $RMAKER_PATH/examples/matter/mfg/keys.csv --mcsv $RMAKER_PATH/examples/matter/mfg/master.csv
```

Configure the passcode as 125 and set the discriminator to 521 to match the QR code on the ESP32-S3-LCD-EV-Board.

This not only generates the factory nvs binary required for matter, but also embeds the RainMaker MQTT Host url into it via the master.csv file. Optionally, you can embed the MQTT host into the firmware itself by using `idf.py menuconfig -> ESP RainMaker Config -> ESP_RMAKER_READ_MQTT_HOST_FROM_CONFIG` and then skipping the --csv and --mcsv options to mfg_tool

The factory binary generated above should be flashed onto the fctry partition (`0x500000` is the fctry partition address).

```
$ esptool.py write_flash 0x500000 $ESP_MATTER_PATH/tools/mfg_tool/out/131b_2/<node-id>/<node-id>-partition.bin
```

### Building the example

Once the environment and required files are set up, we can now proceed to build and flash the example

```
$ idf.py set-target esp32s3
$ idf.py build
$ idf.py flash monitor
```

### Commissioning

To commission the controller, scan the QR Code shown on the screen using ESP RainMaker (iOS App v3.0.0 or later).
> Click the 'setting' icon in the top right corner of the screen and follow tips to reset factory, then the device can be commissioned again.

After commissioning successfully, the Controller could control the other devices (supporting On/Off cluster server) in the same fabric locally by clicking the button on the screen. The type of controlled device can be Matter + Rainmaker light in '$RMAKER_PATH/examples/matter/matter_light' or Matter-only light in '\$ESP_MATTER_PATH/examples/light'.
> For Matter-only light, 'Claiming device certificates' and 'Generating the factory nvs binary' steps are not required, build and flash directly. The QR code for Matter-only light is in the [website](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html#commissioning-and-control).

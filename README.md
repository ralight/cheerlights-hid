A simple C client that subscribes to the Cheerlights MQTT feed and uses ratbagd to update all attached mouse LED colours.

https://github.com/libratbag/libratbag/

## Install

For Ubuntu:

```
sudo apt install ratbagd libmosquitto-dev libglib2.0-dev pkg-config gcc
make
sudo make install
sudo systemctl enable cheerlights-hid
sudo systemctl start cheerlights-hid
```

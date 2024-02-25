# ESPHome External Component for PCF8523 RTC chip

PCF8523 is not a high-precision device, however it's used in some breakouts, especially in [Adalogger FeatherWing - RTC + SD Add-on](https://www.adafruit.com/product/2922)

## Usage

```yaml
external_components:
  - source: github://decayofmind/esphome-pcf8523@master
    components: [ pcf8523 ]
```

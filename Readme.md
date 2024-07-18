# Geosensor device application

Main entry point: 'main.cpp'.

To configure the application update 'config.h'.

Before using this application ensure that the arduino lora chip has a recent firmware version.

## Binary Message Format

The LoRa message is abinary messages including a time followed by n sensor measurements:

Time format (7 bytes):

| Field              | DType              | Description       |
|--------------------|--------------------|-------------------|
| year               | uint16 (2 bytes)   | year (e.g. 2024)  |
| month              | uint8  (1 byte)    | month (1-12)      |
| day                | uint8  (1 byte)    | day (1-31)        |
| hour               | uint8  (1 byte)    | hour (0-23)       |
| minute             | uint8  (1 byte)    | minute (0-59)     |
| second             | uint8  (1 byte)    | second (0-59)     |

Sensor measurements format (9 bytes):

| Field              | DType                  | Description                           |
|--------------------|------------------------|---------------------------------------|
| sensor_id          | unsigned char (1 byte) | unique ID of the soil-moisture sensor |
| water_content      | float  (4 bytes)       | soil moisture in percent              |
| temperature        | float  (4 bytes)       | soil temperature in celsius           |

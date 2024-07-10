# NPK Sensor
<img src="https://github.com/yassinejemaa/watchdog_sleep8s/assets/120018935/eb595ae5-0d62-4633-bc0c-8fd4ae8ea3c9" width="400" />


The sensor must be connected to the RS485 converter via the Data A, Data B, and GND pins, with an external power supply of 12V. However, in our case, a 5V power supply is necessary for the sensor to function correctly. Before starting the measurements, it is crucial to test the sensor by connecting it to the converter as shown in the figure below. To do this, we will use the CoolTerm software to send hexadecimal frames, which will allow us to verify the sensor's proper operation and ensure that the transmitted data is correct.

<img src="https://github.com/yassinejemaa/watchdog_sleep8s/assets/120018935/a0883e81-17bd-4402-90de-77df9c573fdb" width="400" />

This converter is the same as the one illustrated in the previous figure; the only difference is the output pins.

<img src="https://github.com/yassinejemaa/watchdog_sleep8s/blob/main/assets/120018935/88888b7a-4f8b-4d70-8570-b039deeddf72" width="400" />

The DE and RE pins are used to define the direction of information, determining whether our microcontroller will send or receive data.

# Temperature Sensor

<img src="https://github.com/yassinejemaa/watchdog_sleep8s/blob/main/assets/120018935/cbcf335b-dd1c-474f-aed8-2e16aef97dc9" width="400" />

The DS18B20 temperature sensor communicates using the 1-Wire protocol.
# Real Time Clock
<img src="https://github.com/yassinejemaa/watchdog_sleep8s/blob/main/assets/120018935/06cd0284-a0df-472d-a2be-bb0de95ea4ff" width="600" />


In this project, the RTC (Real Time Clock) module must be integrated by assigning it a function in the code that triggers an interrupt at regular intervals. Initially set to a period of 2 weeks, this period has been reduced to 10 seconds for testing purposes.
# LoRa Module
<img src="https://github.com/yassinejemaa/watchdog_sleep8s/blob/main/assets/120018935/89c3b3c2-ca8c-4754-84cf-a93acd658a21" width="400" />

After wiring the RAK4270 module, we need to start by assigning it the necessary coordinates so that it can send data to our server, "The Things Network - TTN".
# TTN Config
<img src="https://github.com/yassinejemaa/watchdog_sleep8s/blob/main/assets/120018935/bf66ca03-4ba0-4932-98c9-be9b46266883" width="400" />


After creating a project in TTN, we need to create our device within the project we have already set up to obtain multiple coordinates.

<img src="https://github.com/yassinejemaa/watchdog_sleep8s/blob/main/assets/120018935/c0e2ce9b-f552-45d8-ac4d-2b6b7a09c3c9" width="400" />

After creating our device, we can easily see the coordinates to input into our node.

<img src="https://github.com/yassinejemaa/watchdog_sleep8s/blob/main/assets/120018935/0cd4e057-43e5-4830-a6a0-ec4200d13cfe" width="400" />

# Rak4270 (LoRa Module) Config
To record the coordinates in our sending module, we need to use the "RAK Serial Port Tool." After connecting the RX and TX pins of the module to the RX and TX pins of our microcontroller, we must connect the reset pin to GND to disable communication from the Arduino board. Then, we can start sending the necessary AT commands, including the coordinates, so that the module can send the data correctly.
<img src="https://github.com/yassinejemaa/watchdog_sleep8s/blob/main/assets/120018935/c151b67f-97e7-4910-8e4b-8c0addd32614" width="400" />


Our RAK module is ready to send data after performing an AT+join each time. It also needs to follow the Cayenne LPP encoding to send multiple measurements in a single message. The message should have a "channel" for each measurement, a declaration of the measurement type, and then the value to send in hex format. In our case, we will send a temperature value on the temperature channel 0 and an analog value on the analog channel 0.

Example: 00 67 + "temperature value" + 00 02 + "analog value"

In general, the message should be written in this format.
<img src="https://github.com/yassinejemaa/watchdog_sleep8s/blob/main/assets/120018935/c26c46aa-6d62-4af7-a6f5-2c2495dc09cb" width="400" />




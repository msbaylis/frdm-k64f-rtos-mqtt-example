
# frdm-k64f-rtos-mqtt-example

MQTT example using the Freescale FRDM-K64F platform without additional hardware.

This is a simple MQTT demo that does the following:

- connect/disconnect to the MQTT Dashboard public broker (broker.mqttdashboard.com)

- subscribe to a user-defined MQTT topic

- publish messages from the FRDM-K64F board when SW2 or SW3 is pressed

- receive messages from a web dashboard client to change the color of the LED

- send serial output regarding connection status and MQTT messages sent/received

The web client code can be found at the following URL:

https://github.com/msbaylis/frdm-k64f-mqtt-dashboard.git

NOTE: make sure you change the #defines in main.cpp to specify your client ID and MQTT topic. I used the MAC address of my FRDM-K64F board without colons for the client ID. Assuming you're board is properly connected to a network, the serial output should display info including the board's MAC address.


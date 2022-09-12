# FlowMeter

For my small motor boat (a Yamarin 56HT), I wanted a way to know how much fuel I used in real-time to keep an eye on my current consumption, but also total consumption during a trip.

Buying and installing a fuel flow meter from Garmin was several hundreds of dollars and quite complicated. I therefore decided to try and make my own.

## Real-time monitoring of fuel flow on my phone via bluetooth

I bought a small flow meter (Hall sensor) of adequate size (I have a 90HP Yamaha 4-stroke engine) and looked at the specs for approximate fuel consumption to find a flow meter within those ranges). A pdf with the specs of the flow meter I boought is available in the project.

I then connected it to the Arduino (if the sensor and arduino are physically near each other, soldering it straight to the arduino without any extra components is fine as the cables are too short to pick up any significant interferences and thus overload the arduino or just malfunction. So it was a very easy and straight-forward connnection. I soldered the sensor to the arduino. Cut my fuel line on the boat and inserted the flow sensor with adequate connections / clamps and made sure there were no fuel leaks. And then connected the arduino to the boat battery (I have USB connectors available on the boat) so that the arduino boots twhen the engine is started.

And finally, I wrote the code so that the arduino takes the data from the flow sensor, converts it to adequate numbers (consumption per hr and per nm, updated every 30 seconds as well as the total consumption). I then use the BLE library to transmit this via Bluetooth. On my phone I use a free Bluetooth monitoring app called "nrfConnect" and connect to my Arduino. (the app is meant to troubleshoot bluetooth connections, but I used it as my interface so I didn't have to program my own mobile app). And so I can watch, in real time my fuel consumption on my phone. 

It was a simple install, cost me an arduino, a flow sensor and some cabling and fuel tubing to connect it all together. (60-70$ total)
Works like a charm and helps me know what power/rpm/speeds to use to minimise fuel consumption as well as how much fuel different trips use up.

Fun and useful project!

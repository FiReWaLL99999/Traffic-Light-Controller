<b>Traffic Light Controller</b><br><br>
The Firmware is done using MPLAB X IDE V3.00. Using compiler XC8.<br>
The Schematic and Simulation are done using SimulIDE.<br>
The project is made using PIC16F877 and not PIC16F877A, as the program used for simulation doesn’t support PIC16F877A.<br>
You can see this video where I showcase the project: https://youtu.be/a3MMaACnfRA<br>

The project features a Traffic Light Controller that has 2 operation modes. Automatic Mode and Manual Mode, which can be switched between them by clicking on “Switch Mode” button.

<b>Automatic Mode:</b><br>
The controller works like the following:
1.	At start: West -> Green (20 seconds), South -> Red (23 seconds)
2.	After 20 seconds: West -> Yellow (3 seconds), South -> Red (3 Seconds)
3.	After 3 seconds: West -> Red (15 seconds), South -> Green (12 seconds)
4.	After 12 seconds: West -> Red (3 seconds), South -> Yellow (3 seconds)
5.	After 3 seconds: West -> Green (20 seconds), South -> Red (23 seconds)
For switching: Upon clicking on the switch button, the program has to wait till it ends its automatic cycle (from 1 to 5) so it can switch to Manual Mode.

<b>Manual Mode:</b><br>
The controller works like the following:
1.	At start: West -> Green, South -> Red
2.	Upon clicking on “Switch Direction” button: West -> Yellow (3 seconds), South -> Red
3.	After 3 Seconds: West -> Red, South -> Green
4.	Upon clicking on “Switch Direction” button: West -> Yellow (3 seconds), South -> Red
5.	After 3 Seconds: West -> Green, South -> Red
For switching: Upon clicking on the switch button, the program has to wait till it ends one of its directions switching cycles (from 2 to 3 or from 4 to 5) so it can switch to Automatic Mode.

<b>Used Components:</b><br>
•	PIC16F877 Microcontroller<br>
•	4 Common Cathode Red LED 7-Segments<br>
•	4 ICs 4778 (for decoding from BCD to 7-Segments code)<br>
•	6 LEDS (2 Green, 2 Red, 2 Yellow)<br>
•	2 Push Buttons<br>
•	34 Resistors<br>

🎮 RGB GUARDIAN – Arduino RGB Arcade Game

An Original DIY Electronics Project by Aman Verma

Created & Developed by: Aman Verma
Brand: VERMA ELECTRONIC LAB

RGB GUARDIAN is an Arduino-based colour-matching arcade game developed using WS2812B / NeoPixel addressable RGB LEDs. Players use three colour buttons to shoot matching enemies and score points.

This is my original DIY electronics project, designed, developed and programmed by me.

🚀 Features

36 RGB Pixel LED game display

Red, Green and Blue buttons

Buzzer sound effects

3 Lives system

5 Difficulty levels

Combo scoring system

Automatic enemy colour sequence

Start screen and button-based restart

Victory animation at 500 points

Game Over animation

🧰 Components Required

Component

Quantity

Arduino Nano

1

WS2812B / NeoPixel LEDs

36

Red Push Button

1

Green Push Button

1

Blue Push Button

1

Buzzer

1

5V Power Supply

1

Connecting Wires

As required

📌 Pin Connections

Component

Arduino Pin

Red Button

D2

Green Button

D3

Blue Button

D4

Buzzer

D5

LED Data

D6

📚 Required Library

This project requires the Adafruit NeoPixel library.

Installation Steps

Open Arduino IDE.

Go to Sketch → Include Library → Manage Libraries.

Search for Adafruit NeoPixel.

Install Adafruit NeoPixel by Adafruit.

Open the RGB_Guardian.ino file.

Select Arduino Nano and upload the code.

Library: https://github.com/adafruit/Adafruit_NeoPixel

🔧 Customize LED Count

You can use different numbers of Pixel LEDs according to your requirements.

Find this line in the Arduino code:

#define NUM_LEDS 36

Change the number according to your LED count.

Examples:

#define NUM_LEDS 36
#define NUM_LEDS 60
#define NUM_LEDS 100
#define NUM_LEDS 150

Important: Use the actual number of connected LEDs and an appropriately rated 5V power supply. Higher LED counts require more power.

⚡ Power Supply Notes

Use a regulated 5V power supply.

Select sufficient current according to LED quantity and brightness.

Connect Arduino GND and LED power supply GND together.

Connect LED strip DIN to Arduino D6.

Buttons use INPUT_PULLUP mode.

🎮 How to Play

Power ON the Arduino.

Wait for the Start Screen.

Press any colour button to start.

Match the enemy colour with the correct button.

Destroy enemies to earn points.

Complete all 5 levels.

Reach 500 points to WIN.

Lose all 3 lives and the game ends.

Press any button to restart.

👨‍💻 Original Creator

Aman Verma
VERMA ELECTRONIC LAB

This project is independently developed by me as an original DIY electronics project.

If you share or showcase this project, please give proper credit to the original creator.

© 2026 Aman Verma | VERMA ELECTRONIC LAB

⭐ Support

If you like this project, give this repository a ⭐ Star on GitHub!

What do you do when your very, very old Thrustmaster Cougar stick dies, but you're left with the perfectly good throttle? Pretty obvious. Pick up a microcontroller, do a bit of soldering, and get writing.

This is an arduino app that uses a SAMD21 board (in my case the Adafruit QT Py), and an MCP23X08 IO extender to handle all the buttons the throttle has.

It presents the throttle as a USB HID controller with 5 analog axis and 10 digital buttons.


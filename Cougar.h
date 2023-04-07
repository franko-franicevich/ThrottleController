#ifndef _COUGAR_H_
#define _COUGAR_H_

#include "utilities.h"
#include <Smoothed.h>   // We'll use this to smooth out the analog inputs.

/* Actual Cougar HID descriptor obtained using dump from 'mac-hid-dump' utility.
There are three devices, which is interesting. I expected 2. Joystick & throttle.
The first and last are interesting: The first a mouse, the last a keyboard.
I've vague memories of powerful config options where you could at least map
skeystrokes instead of buttons in different profiles. Was there a mouse option too?

The middle one seeems to be the throttle and joystick combined in to one report.

044F 0400: Thrustmaster - Thrustmaster HOTAS Cougar
DESCRIPTOR:
  05  01  09  02  a1  01  09  01  a1  00  05  09  19  01  29  03
  15  00  25  01  95  03  75  01  81  02  95  01  75  05  81  01
  05  01  09  30  09  31  09  38  15  80  25  7f  75  08  95  03
  81  06  c0  c0
  (52 bytes)

044F 0400: Thrustmaster - Thrustmaster HOTAS Cougar
DESCRIPTOR:
  05  01  09  04  a1  01  05  01  09  01  a1  00  05  09  19  01
  29  1c  15  00  25  01  75  01  95  1c  81  02  75  10  95  01
  81  01  05  01  09  39  25  07  35  00  46  0e  01  66  40  00
  75  04  81  42  09  30  66  00  00  15  00  27  ff  ff  00  00
  47  ff  ff  00  00  75  10  95  01  81  02  09  31  81  02  09
  32  16  00  00  26  ff  00  36  ff  00  45  00  75  08  81  02
  09  33  81  02  09  36  81  02  09  35  81  02  09  37  81  02
  09  34  81  02  c0  c0
  (118 bytes)

044F 0400: Thrustmaster - Thrustmaster HOTAS Cougar
DESCRIPTOR:
  05  01  09  06  a1  01  05  07  75  08  95  10  05  07  19  00
  29  ff  15  00  26  ff  00  81  00  c0
  (26 bytes)

These translates to:

Seems to represent a 3 button mouse with a scroll wheel?Mouse?:
0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
0x09, 0x02,        // Usage (Mouse)
0xA1, 0x01,        // Collection (Application)
0x09, 0x01,        //   Usage (Pointer)
0xA1, 0x00,        //   Collection (Physical)
0x05, 0x09,        //     Usage Page (Button)
0x19, 0x01,        //     Usage Minimum (0x01)
0x29, 0x03,        //     Usage Maximum (0x03)
0x15, 0x00,        //     Logical Minimum (0)
0x25, 0x01,        //     Logical Maximum (1)
0x95, 0x03,        //     Report Count (3)
0x75, 0x01,        //     Report Size (1)
0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x95, 0x01,        //     Report Count (1)
0x75, 0x05,        //     Report Size (5)
0x81, 0x01,        //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
0x09, 0x30,        //     Usage (X)
0x09, 0x31,        //     Usage (Y)
0x09, 0x38,        //     Usage (Wheel)
0x15, 0x80,        //     Logical Minimum (-128)
0x25, 0x7F,        //     Logical Maximum (127)
0x75, 0x08,        //     Report Size (8)
0x95, 0x03,        //     Report Count (3)
0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
0xC0,              //   End Collection
0xC0,              // End Collection

// 52 bytes


and Throttle:
0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
0x09, 0x04,        // Usage (Joystick)
0xA1, 0x01,        // Collection (Application)
0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
0x09, 0x01,        //   Usage (Pointer)
0xA1, 0x00,        //   Collection (Physical)
0x05, 0x09,        //     Usage Page (Button)
0x19, 0x01,        //     Usage Minimum (0x01)
0x29, 0x1C,        //     Usage Maximum (0x1C)
0x15, 0x00,        //     Logical Minimum (0)
0x25, 0x01,        //     Logical Maximum (1)
0x75, 0x01,        //     Report Size (1)
0x95, 0x1C,        //     Report Count (28)
0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x75, 0x10,        //     Report Size (16)
0x95, 0x01,        //     Report Count (1)
0x81, 0x01,        //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
0x09, 0x39,        //     Usage (Hat switch)
0x25, 0x07,        //     Logical Maximum (7)
0x35, 0x00,        //     Physical Minimum (0)
0x46, 0x0E, 0x01,  //     Physical Maximum (270)
0x66, 0x40, 0x00,  //     Unit (Length: Degrees)
0x75, 0x04,        //     Report Size (4)
0x81, 0x42,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State)
0x09, 0x30,        //     Usage (X)
0x66, 0x00, 0x00,  //     Unit (None)
0x15, 0x00,        //     Logical Minimum (0)
0x27, 0xFF, 0xFF, 0x00, 0x00,  //     Logical Maximum (65534)
0x47, 0xFF, 0xFF, 0x00, 0x00,  //     Physical Maximum (65534)
0x75, 0x10,        //     Report Size (16)
0x95, 0x01,        //     Report Count (1)
0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x09, 0x31,        //     Usage (Y)
0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x09, 0x32,        //     Usage (Z)
0x16, 0x00, 0x00,  //     Logical Minimum (0)
0x26, 0xFF, 0x00,  //     Logical Maximum (255)
0x36, 0xFF, 0x00,  //     Physical Minimum (255)
0x45, 0x00,        //     Physical Maximum (0)
0x75, 0x08,        //     Report Size (8)
0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x09, 0x33,        //     Usage (Rx)
0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x09, 0x36,        //     Usage (Slider)
0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x09, 0x35,        //     Usage (Rz)
0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x09, 0x37,        //     Usage (Dial)
0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x09, 0x34,        //     Usage (Ry)
0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0xC0,              //   End Collection
0xC0,              // End Collection

// 118 bytes

// Keyboard.

0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
0x09, 0x06,        // Usage (Keyboard)
0xA1, 0x01,        // Collection (Application)
0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
0x75, 0x08,        //   Report Size (8)
0x95, 0x10,        //   Report Count (16)
0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
0x19, 0x00,        //   Usage Minimum (0x00)
0x29, 0xFF,        //   Usage Maximum (0xFF)
0x15, 0x00,        //   Logical Minimum (0)
0x26, 0xFF, 0x00,  //   Logical Maximum (255)
0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
0xC0,              // End Collection

// 26 bytes

In the end, while fascinating, it's not directly usable, as it combines the throttle
and joystick in to one mega-device. It is useful though, since I can map my analog inputs
in to those usage types. Though there seems to be one extra analog defined. Virtual?
Reserved for the optional rudder controller? 

Another useful tidbit was that all the throttle analog inputs are just 8 bit precision.
Which, given the high jitter on these, makes a lot of sense. There range of values we're
reading off the controller is also quite 'compressed', with min val and max val taking 
only about half the entire bitspace for some of the analog inputs (especially throttle)
We'll remap those before sending.

Some useful HID usage types:
30  X
31  Y
32  Z
33  Rx
34  Ry
35  Rz
36  Slider
37  Dial
38  Wheel
39  HAT switch
BA  Rudder
BB  Throttle
*/

// Thanks for ideas here:
// https://giovannimedici.wordpress.com/2020/08/28/standalone-usb-adapter-for-tm-cougar-throttle/

static uint8_t cougar_report_desc[] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    //0x09, 0x05,                    // USAGE (Gamepad)
    0x09, 0x04,                    // USAGE (Joystick)
    0xA1, 0x01,                    // COLLECTION (Application)

    // The cursor 2 analog input mapped as a pointer with 8 bit precision: MX/MY
    0x05, 0x01,                    //   Usage Page (Generic Desktop)
//    0x09, 0x01,                    //     Usage (Pointer)
    0xA1, 0x00,                    //     COLLECTION (Physical)
    0x09, 0x33,                    //      USAGE (Rx) Cursor
    0x09, 0x34,                    //      USAGE (Ry) Cursor 
    0x16, 0x00, 0x00,              //      Logical Minimum (0)
    0x26, 0xFF, 0x00,              //      Logical Maximum (255) 8 bit. The cursor is pretty jittery and not precise.
    0x75, 0x08,                    //      Report Size (8)
    0x95, 0x02,                    //      REPORT_COUNT (2)
    0x81, 0x02,                    //      INPUT (Data,Var,Abs)
    0xc0,                          //     END_COLLECTION

    // the other analog inputs. ANT and RNG, along with throttle
    // All 8 bit, like original. Precision is more than good enough here given jitter in readings.
    //0x09, 0xbb,                    //      USAGE (bb) Throttle
    0x09, 0x32,                    //      USAGE Z (32)   (many apps will map this to throttle anyway) Could use 0x36 (slider) instead
    0x09, 0x35,                    //      USAGE (Rz)
    0x09, 0x31,                    //      USAGE 31 (Y)
    0x95, 0x03,                    //      REPORT_COUNT (3)
    0x81, 0x02,                    //      INPUT (Data,Var,Abs)

    // The Buttons
    0x05, 0x09,                    // Usage Page (Button)
    0x15, 0x00,                    // Logical Minimum (0)
    0x25, 0x01,                    // Logical Maximum (1)
    0x19, 0x01,                    // Usage Minimum (Button #1)
    0x29, 0x0A,                    // Usage Maximum (Button #10)
    0x75, 0x01,                    // Report Size (1)
    0x95, 0x0A,                    // Report Count (10)
    0x81, 0x02,                    // INPUT (Data,Var,Abs)

    0x75, 0x06,                    //      REPORT_SIZE () Padding Zeros to bring this to the next byte boundary
    0x95, 0x01,                    //      REPORT_COUNT (1)
    0x81, 0x03,                    //      INPUT (Cnst,Var,Abs)

    0xc0                           // END_COLLECTION
};


/* This c struct matches the bit packing of the above HID descriptor.
 All we need to do is set the fields then copy this as part of the USB
 data packet. 
 */
#pragma pack(push,1)      // ensure we're packing only to byte boundaries, not words.
typedef struct {
    // 1st 32 bit word
    uint8_t mx           ; // Rx axis 
    uint8_t my           ; // Ry axis 
    uint8_t throttle     ; // Throttle (bb axis)
    uint8_t ant          ; // Rz axis

    // 2nd word
    uint8_t rng          ; // Dial axis
    uint16_t buttons : 10; // 10 buttons (1 bit each)
    uint16_t padding : 6;  // padding to align to 8-bit boundary
} CougarHIDReport_t;
#pragma pack(pop)

// Some of our analog inputs don't cover the whole range
// of 0-255. So this struct helps us define the lowest
// and highest values that we read, which will be remapped
// to the range 0 and 255.
struct analogMapping_t {
  uint8_t min;
  uint8_t max;
};

// these are what we've seen through testing to be the extreme
// read limits for each input.
analogMapping_t throttleMap = {25, 225};
analogMapping_t mxMap = {75, 200};
analogMapping_t myMap = {65, 190};
analogMapping_t antMap = {0, 255};
analogMapping_t rngMap = {0, 255};

/* A class to help us pack the CougarHIDReport_t c struct. Often the bitpacking
 can be complex, but since we're mostly using 8 bit values, the only tricky
 thing for us here is setting the bitmask for the buttons.

 The class also contains helper functions to debounce the buttons, and detect
 when the inputs have changed, so that we know when to send an HID update.
 */
class CougarHIDReport {
  public:
    // Constructor
    CougarHIDReport() : report{}, old_report{}, debounce{}, readResolution{8} {
      // The Smoothed library will allow us to do rolling exponential
      // or simple averages for the analog inputs, reduing jitter at
      // the cost of slight latency. The higher the number, the more weight
      // is given to the most recent reading. ie, less latency, more jitter.
      throttle.begin(SMOOTHED_EXPONENTIAL, 60); // Weighting is 60% new value, 40% older. Balance for latency vs jitter.
      mx.begin(SMOOTHED_EXPONENTIAL, 60);
      my.begin(SMOOTHED_EXPONENTIAL, 60);
      rng.begin(SMOOTHED_EXPONENTIAL,60);
      ant.begin(SMOOTHED_EXPONENTIAL,60);
    }

    // Set button state. Assume buttons start at 1, not 0 index.
    // This function also handles debouncing. A button state is
    // not changed unless it's state is changed twice in a row.
    void setButton(uint8_t button, bool state) {
      if ( button < 0 || button >= 10)
        return;

      // We store *potential* button 'changes' in the 'debounce' struct. 
      // The debounce struct basically holds what we read for the button last
      // time. We don't update the report until the stored debounce matches the
      // new state. ie; we've gotten the same value when reading the button 
      // twice in a row.

      // Check if debounce and the new state match. If so, we update the report
      if (bitRead(debounce, button) == state)
        bitWrite(report.buttons, button, state);

      // Now save the value as the 'debounce' check for next time.
      bitWrite(debounce, button, state);
    }

    // We may be reading aty a different, higher resolution than the HID 
    // supports. This is useful for the rolling average calculation, giving
    // us higher precision. This function allows us to downsample back in
    // to the required 8 bit range for the HID struct
    // We also ignore any change to the value which is smaller than the
    // threshold. Stops the +1/-1 jitter that still creeps through, BUT
    // doesn't introduce latency for BIG changes.
    uint8_t analogResolutionAdjust(uint16_t value, uint8_t oldvalue, uint8_t threshold, analogMapping_t mapping){
      if (readResolution > 8)
        /// Bitshift the value by the number of bits needed to shrink it to 8bit
        value = value >> (readResolution - 8);
      
      value = map(value, mapping.min, mapping.max, 0, 255);

      // Discard changes that are smaller than our threshold.
      if ( abs(value-oldvalue) <= threshold)
        return oldvalue;
      return value;
    }

    // Set X axis value
    void setMx(uint16_t value) {
      mx.add(value);
      report.mx = analogResolutionAdjust(mx.get(), report.mx, 2, mxMap);
    }

    // Set Y axis value
    void setMy(uint16_t value) {
      my.add(value);
      report.my = analogResolutionAdjust(my.get(), report.my, 2, myMap);
    }

    // Set Rx axis value
    void setAnt(uint16_t value) {
      ant.add(value);
      report.ant = analogResolutionAdjust(ant.get(), report.ant, 1, antMap);
    }

    // Set Ry axis value
    void setRng(uint16_t value) {
      rng.add(value);
      report.rng = analogResolutionAdjust(rng.get(), report.rng, 1, rngMap);
    }

    // Set Throttle value
    void setThrottle(uint16_t value) {
      throttle.add(value);  // Throttle seems inverted.
      report.throttle = analogResolutionAdjust(throttle.get(), report.throttle, 1, throttleMap);
    }

    // Get the raw report data
    const uint8_t* getReportData() const {
      return reinterpret_cast<const uint8_t*>(&report);
    }
    const size_t getReportSize() {
      return sizeof(CougarHIDReport_t);
    }
    
    bool operator==(const CougarHIDReport_t& rhs)
    {
      return !memcmp(&report, (void*)&rhs, sizeof(CougarHIDReport_t));
    }

    bool operator!=(const CougarHIDReport_t& rhs)
    {
      return memcmp(&report, (void*)&rhs, sizeof(CougarHIDReport_t));
    }

    bool operator=(const CougarHIDReport_t& rhs)
    {
      return memcpy(&report, (void*)&rhs, sizeof(CougarHIDReport_t));
    }

    uint16_t buttons() { return report.buttons;}

    // Dump the current state of the inputs to serial
    void dumpState(){ dumpState(report); }
    void dumpState(const CougarHIDReport_t &state){
      char s1[11];
      uint16_to_binary_string(state.buttons, s1, 11);
      Serial.printf("%s   %4u %4u %4u %4u %4u", s1, state.throttle, state.rng, state.ant, state.mx, state.my);
    }


    /* Has the state of the controller changed since the last time
    we checked?
    Have the analog sticks moved? Have buttons been pressed/released?
    This function returns true if the inputs are different from the
    LAST time hasChanged() been called. Basically, updates are reset
    when this is called. 
    */
    bool hasChanged(){
      return *this != old_report;
    }

    // We've sent the report with changes, so now we reset, and start
    // detecting changes again.
    void changesSent()
    {
      memcpy(&old_report, &report, sizeof(CougarHIDReport_t));
    }

    /* Do the analog inputs on OldReport differ from the potential new report? */
    bool detectAnalogChanges() {
      return !memcmp(&report, &old_report, getReportSize());
    }

    /* Do the buttons differ from the previously sent report? */
    bool detectButtonChanges() {
      // Make sure there's no high order unitialised junk, as we're only comparing the lower 10 bits.
      return (report.buttons & 0b1111111111) != (old_report.buttons & 0b1111111111);
    }

    bool reportCompare(const CougarHIDReport_t &lhs, const CougarHIDReport_t &rhs){
      return !memcmp(&lhs, (void*)&rhs, sizeof(CougarHIDReport_t));
    }

    // returns true if any button is currently depressed.
    bool isAnyButtonOn()
    {
      return report.buttons & 0b1111111111;
    }

    void setReadResolution(uint8_t resolution){
      readResolution = resolution;
    }

  private:
    CougarHIDReport_t report;
    CougarHIDReport_t old_report;

    uint16_t debounce = 0; // The set of changes stored to check for debounce.

    // read resolution is the resolution that we're reading off the analog inputs
    // If it's greater than our 8 bit HID resolution, we'll downsample prior to sending.
    uint8_t readResolution = 8; 

    // The smoothed analog values
    Smoothed<uint16_t> throttle;
    Smoothed<uint16_t> ant;
    Smoothed<uint16_t> rng;
    Smoothed<uint16_t> mx;
    Smoothed<uint16_t> my;

};

#endif
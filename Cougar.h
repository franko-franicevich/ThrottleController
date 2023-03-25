#ifndef _COUGAR_H_
#define _COUGAR_H_

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
Which, given the high jitter on these, makes a lot of sense. There range is also quite
'compressed', with min val and max val taking only about half the entire bitspace.
We'll remap before sending.

*/


// Thanks for ideas here:
// https://giovannimedici.wordpress.com/2020/08/28/standalone-usb-adapter-for-tm-cougar-throttle/

static uint8_t cougar_report_desc[] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    //0x09, 0x05,                    // USAGE (Gamepad)
    0x09, 0x04,                    // USAGE (Joystick)
    0xA1, 0x01,                    // COLLECTION (Application)

    // The Buttons
    0x05, 0x09,                    // Usage Page (Button)
    0x15, 0x00,                    // Logical Minimum (0)
    0x25, 0x01,                    // Logical Maximum (1)
    0x19, 0x01,                    // Usage Minimum (Button #1)
    0x29, 0x0A,                    // Usage Maximum (Button #10)
    0x75, 0x01,                    // Report Size (1)
    0x95, 0x0A,                    // Report Count (10)
    0x81, 0x02,                    // INPUT (Data,Var,Abs)

    // The cursor 2 analog input mapped as a pointer with 10 bit precision: MX/MY
    0x05, 0x01,                    //   Usage Page (Generic Desktop)
    0x09, 0x01,                    //     Usage (Pointer)
    0xA1, 0x00,                    //     COLLECTION (Physical)
    0x09, 0x30,                    //      USAGE (X) Cursor
    0x09, 0x31,                    //      USAGE (Y) Cursor 
    0x15, 0x00,                    //      Logical Minimum (0)
    0x26, 0xFF, 0x03,              //      Logical Maximum (1023) 03FF = 1023, 10 bit. The cursor is pretty jittery and not precise. Even 10 bit is overkill. 
    0x75, 0x0A,                    //      Report Size (10)
    0x95, 0x03,                    //      REPORT_COUNT (2)
    0x81, 0x02,                    //      INPUT (Data,Var,Abs)
    0xc0,                          //     END_COLLECTION

    // 2 bit Padding to fill out the 32 bit word
    0x75, 0x02,                    //      REPORT_SIZE (2) Padding Zeros to bring this to the next int32 boundary
    0x95, 0x01,                    //      REPORT_COUNT (1)
    0x81, 0x03,                    //      INPUT (Cnst,Var,Abs)

    // the other two analog inputs. ANT and RNG, along with throttle
    // All 10 bit. Precision is more than good enough here given jitter in readings.
    // I considered 12 bit, but it's kind of pointless given the precision of the reading.
    // The ANT and RNG were by far the most precise, but even they don't benefit much from 12 bit.
    // In the end, made the throttle 12 bit, to fit this all neatly in one 32 bit word.
    // I'll use a smoothing library/rolling average to clean this up. The latency impact should be minor
    0x09, 0x34,                    //      USAGE (Rx)
    0x09, 0x33,                    //      USAGE (Ry)
    0x15, 0x00,                    //      Logical Minimum (0)
    0x26, 0xFF, 0x03,              //      Logical Maximum (1023) 03FF = 1023, 10 bit.
    0x75, 0x0A,                    //      Report Size (10)
    0x95, 0x01,                    //      REPORT_COUNT (2)
    0x81, 0x02,                    //      INPUT (Data,Var,Abs)

    //0x09, 0xbb,                    //      USAGE (bb) Throttle
    0x09, 0x32,                    //      USAGE z    (many apps will map this to throttle anyway)
    0x75, 0x0C,                    //      REPORT_SIZE (12)
    0x95, 0x01,                    //      REPORT_COUNT (1)
    0x15, 0x00,                    //      Logical Minimum (0)
    0x26, 0xff, 0x0f,              //       LOGICAL_MAXIMUM (4095)  12 bit
    0x81, 0x02,                    //      INPUT (Data,Var,Abs)

    0xc0                           // END_COLLECTION
};

typedef struct {
    uint16_t buttons    : 10; // 10 buttons (1 bit each)
    uint16_t x          : 10; // X axis (10-bit)
    uint16_t y          : 10; // Y axis (10-bit)
    uint16_t padding    : 2;  // 2-bit padding to align to 32-bit boundary
    uint16_t rx         : 10; // Rx axis (10-bit)
    uint16_t ry         : 10; // Ry axis (10-bit)
    uint16_t throttle   : 12; // Throttle (Z axis, 12-bit)
} CougarHIDReport_t;

class CougarHIDReport {
  public:
    // Constructor
    CougarHIDReport() : report{} {}

    // Set button state. Assume buttons start at 1, not 0 index.
    void setButton(uint8_t button, bool state) {
      if (button > 0 && button <= 10)
        bitWrite(report.buttons, button, state);
    }

    // Set X axis value
    void setX(uint16_t value) {
      report.x = value & 0x03FF;
    }

    // Set Y axis value
    void setY(uint16_t value) {
      report.y = value & 0x03FF;
    }

    // Set Rx axis value
    void setRx(uint16_t value) {
      report.rx = value & 0x03FF;
    }

    // Set Ry axis value
    void setRy(uint16_t value) {
      report.ry = value & 0x03FF;
    }

    // Set Throttle value
    void setThrottle(uint16_t value) {
      report.throttle = value & 0x0FFF;
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

    bool operator=(const CougarHIDReport_t& rhs)
    {
      return !memcpy(&report, (void*)&rhs, sizeof(CougarHIDReport_t));
    }



  private:
    CougarHIDReport_t report;
};

#endif
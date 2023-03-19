#ifndef _COUGAR_H_
#define _COUGAR_H_

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
    0x09, 0x32,                    //      USAGE z
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
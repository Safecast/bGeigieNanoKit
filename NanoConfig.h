#ifndef _NANO_CONFIG_H_
#define _NANO_CONFIG_H_

//
// bGeigie Nano definitions
//

#define NANO_DEVICE_ID        0000
#define NANO_VERSION       "2.0.1"
#define NANO_HEADER        "BNRDD"
#define NANO_CPM_FACTOR        334
#define NANO_BQM2_FACTOR        37

//
// Enable or Disable features
//

#define ENABLE_DEBUG             0
#define ENABLE_DIAGNOSTIC        0
#define ENABLE_SLEEPMODE         0
#define ENABLE_SSD1306           1
#define ENABLE_SOFTGPS           0
#define ENABLE_STATIC_GPS        1
#define ENABLE_HARDWARE_COUNTER  0
#define ENABLE_OPENLOG           0
#define ENABLE_WAIT_GPS_FOR_LOG  1
#define ENABLE_GPS_NMEA_LOG      0
#define ENABLE_100M_TRUNCATION   0
#define ENABLE_MEDIATEK          1
#define ENABLE_SKYTRAQ           0
#define ENABLE_EEPROM_DOSE       1
#define ENABLE_CUSTOM_FN         1 // enable custom function button
#define ENABLE_LND_DEADTIME      1 // enable dead-time compensation for LND7317
#define ENABLE_GEIGIE_SWITCH     1 // switch between bGeigie and xGeigie type
#define ENABLE_NANOKIT_PIN       1 // use the nano kit configuration
#define ENABLE_NANOPCBKIT_PIN    0 // use the nano pcb kit configuration
#define ENABLE_INTEGRATEDNANOKIT_PIN    1 // use the integrated nano pcb kit configuration

#if ENABLE_SSD1306 // high memory usage (avoid logs)
#undef ENABLE_DEBUG // disable debug log output
#endif

//
// Pins definition
//

#if ENABLE_NANOKIT_PIN
#if ENABLE_NANOPCBKIT_PIN
  #warning NANO PCB KIT with OLED screen used !
  #define OLED_SPI_MODE // SPI mode enabled
  #define OLED_CLK 10
  #define OLED_DATA 9
  #define OLED_DC 11
  #define OLED_CS 12
  #define OLED_RESET 13
  #define GPS_RX_PIN 8
  #define GPS_TX_PIN 7
  #define OPENLOG_RX_PIN 6
  #define OPENLOG_TX_PIN 5
  #define OPENLOG_RST_PIN 4
  #define LOGALARM_LED_PIN A4
  #define CUSTOM_FN_PIN 3
#elif ENABLE_INTEGRATEDNANOKIT_PIN
  #warning INTEGRATED NANO PCB used!
  #define OLED_RESET D7
  #define I2C1_SDA D0
  #define I2C1_SCL D1
  #define SPI3_MOSI D2
  #define SPI3_MISO D3
  #define SPI3_SCK D4
  #define CS_LOG D5
  #define PWR_ON D6
  #define LCD_RST D7
  #define RX_GPS D8
  #define TX_GPS D9
  #define IROVER D10
  #define ENC_SW D11
  #define BAT_MEASURE D12
  #define PWR_OFF D13
  #define ENC_A D14
  #define ENC_B D15
  #define RX_XBEE D16
  #define TX_XBEE D17
#else
  #warning NANO KIT with OLED screen used !
  #define OLED_SPI_MODE // SPI mode enabled
  #define OLED_CLK 7
  #define OLED_DATA 6
  #define OLED_DC 5
  #define OLED_CS 4
  #define OLED_RESET 3
  #define GPS_RX_PIN 8
  #define GPS_TX_PIN 9
  #define OPENLOG_RX_PIN 10
  #define OPENLOG_TX_PIN 11
  #define OPENLOG_RST_PIN 12
#endif  // ENABLE_NANOPCBKIT_PIN
#else
#if ENABLE_HARDWARE_COUNTER
  // Pin assignment for version 1.0.1
  #warning Hardware counter is used !
  #define OLED_RESET 4
  #define GPS_RX_PIN 6
  #define GPS_TX_PIN 7
  #define OPENLOG_RX_PIN 8
  #define OPENLOG_TX_PIN 9
  #define OPENLOG_RST_PIN 10
#else
  // Old Pin assignment for version 1.0.0
  #warning Interrupt counter is used !
  #define OLED_RESET 4
  #define GPS_RX_PIN 5
  #define GPS_TX_PIN 6
  #define OPENLOG_RX_PIN 7
  #define OPENLOG_TX_PIN 8
  #define OPENLOG_RST_PIN 9
#endif
#endif


// HardwareCounter pin
// the timer1 pin on the 328p is D5
#define HARDWARE_COUNTER_TIMER1 5

// InterruptCounter pin
// 0 = D2, 1 = D3
#define INTERRUPT_COUNTER_PIN 0

// bGeigie <-> xGeigie switch pin
#define GEIGIE_TYPE_PIN A5
#define GEIGIE_TYPE_THRESHOLD 500

// Voltage divider
// GND -- R2 --A0 -- R1 -- VCC
// https://en.wikipedia.org/wiki/Voltage_divider
#define VOLTAGE_PIN BAT_MEASURE
#define VOLTAGE_R1 9100
#define VOLTAGE_R2 1000

#endif

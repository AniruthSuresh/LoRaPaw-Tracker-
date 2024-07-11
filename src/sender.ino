/*feel free to contact
 * sreeramaj53@gmail.com
 * www.youtube.com/ZenoModiff
   * last updated - time 07:45 PM - date 27 may 2021
 Github Link :-- https://github.com/Zenomodiff/RFM95-LoRa-Module-Interfacing
   */

#include <SPI.h>
#include <LoRa.h>


int counter = 0;
void setup() {
  Serial.begin(9600);
   while (!Serial);

  Serial.println("LoRa Sender");

  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  
}

void loop() {
  
  Serial.print("Sending packet no: ");
  Serial.println(counter);

  LoRa.beginPacket();
  LoRa.print("NYPD ");  
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;

  delay(2000);
}
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <LoRa.h>

//
// For normal use, we require that you edit the sketch to replace FILLMEIN
// with values assigned by the TTN console. However, for regression tests,
// we want to be able to compile these scripts. The regression tests define
// COMPILE_REGRESSION_TEST, and in that case we define FILLMEIN to a non-
// working but innocuous value.
//
#ifdef COMPILE_REGRESSION_TEST
# define FILLMEIN 0
#else
# warning "You must replace the values marked FILLMEIN with real values from the TTN control panel!"
# define FILLMEIN (#dont edit this, edit the lines that use FILLMEIN)
#endif

// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8]={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8]={0x2B, 0x6D, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
static const u1_t PROGMEM APPKEY[16] = {0x52, 0xE2, 0xFF, 0x93, 0xF9, 0xD5, 0x0D, 0x18, 0x7B, 0xF1, 0x4B, 0xFB, 0x1D, 0x6D, 0xAC, 0x55};
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}

uint8_t mydata[2] = {highByte(0),lowByte(0)};
static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 10;

// Pin mapping
const lmic_pinmap lmic_pins = {
    .nss = 5,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 15,
    .dio = {2,4, LMIC_UNUSED_PIN},
};

void printHex2(unsigned v) {
    v &= 0xff;
    if (v < 16)
        Serial.print('0');
    Serial.print(v, HEX);
}

void onEvent (ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            Serial.println(F("EV_SCAN_TIMEOUT"));
            break;
        case EV_BEACON_FOUND:
            Serial.println(F("EV_BEACON_FOUND"));
            break;
        case EV_BEACON_MISSED:
            Serial.println(F("EV_BEACON_MISSED"));
            break;
        case EV_BEACON_TRACKED:
            Serial.println(F("EV_BEACON_TRACKED"));
            break;
        case EV_JOINING:
            Serial.println(F("EV_JOINING"));
            break;
        case EV_JOINED:
            Serial.println(F("EV_JOINED"));
            {
              u4_t netid = 0;
              devaddr_t devaddr = 0;
              u1_t nwkKey[16];
              u1_t artKey[16];
              LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
              Serial.print("netid: ");
              Serial.println(netid, DEC);
              Serial.print("devaddr: ");
              Serial.println(devaddr, HEX);
              Serial.print("AppSKey: ");
              for (size_t i=0; i<sizeof(artKey); ++i) {
                if (i != 0)
                  Serial.print("-");
                printHex2(artKey[i]);
              }
              Serial.println("");
              Serial.print("NwkSKey: ");
              for (size_t i=0; i<sizeof(nwkKey); ++i) {
                      if (i != 0)
                              Serial.print("-");
                      printHex2(nwkKey[i]);
              }
              Serial.println();
            }
            // Disable link check validation (automatically enabled
            // during join, but because slow data rates change max TX
	    // size, we don't use it in this example.
            LMIC_setLinkCheckMode(0);
            break;
        /*
        || This event is defined but not used in the code. No
        || point in wasting codespace on it.
        ||
        || case EV_RFU1:
        ||     Serial.println(F("EV_RFU1"));
        ||     break;
        */
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            break;
        case EV_TXCOMPLETE:
            Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println(F("Received ack"));
            if (LMIC.dataLen) {
              Serial.print(F("Received "));
              Serial.print(LMIC.dataLen);
              Serial.println(F(" bytes of payload"));
            }
            // Schedule next transmission
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
            break;
        case EV_LOST_TSYNC:
            Serial.println(F("EV_LOST_TSYNC"));
            break;
        case EV_RESET:
            Serial.println(F("EV_RESET"));
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            Serial.println(F("EV_RXCOMPLETE"));
            break;
        case EV_LINK_DEAD:
            Serial.println(F("EV_LINK_DEAD"));
            break;
        case EV_LINK_ALIVE:
            Serial.println(F("EV_LINK_ALIVE"));
            break;
        /*
        || This event is defined but not used in the code. No
        || point in wasting codespace on it.
        ||
        || case EV_SCAN_FOUND:
        ||    Serial.println(F("EV_SCAN_FOUND"));
        ||    break;
        */
        case EV_TXSTART:
            Serial.println(F("EV_TXSTART"));
            break;
        case EV_TXCANCELED:
            Serial.println(F("EV_TXCANCELED"));
            break;
        case EV_RXSTART:
            /* do not print anything -- it wrecks timing */
            break;
        case EV_JOIN_TXCOMPLETE:
            Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
            break;

        default:
            Serial.print(F("Unknown event: "));
            Serial.println((unsigned) ev);
            break;
    }
}


void do_send(osjob_t* j) {
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    }
    else{
      // LoRa.setPins(5,15,2);
      if (!LoRa.begin(868E6)) {
        Serial.println("Starting LoRa failed!");
        // while (1);
      }
      int packetSize = 0;
      while (packetSize == 0) {
          // Continuously listen for LoRa packets until one is received
          packetSize = LoRa.parsePacket();
          if (packetSize) {
              int rssi = -LoRa.packetRssi();
              Serial.println(rssi);
              uint8_t payload[2] = { highByte(rssi), lowByte(rssi) };
              LMIC_setTxData2(1, payload, sizeof(payload), 0);
              Serial.println(F("Packet queued"));
              // LoRa.end();
              break;
          }
      }
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

// void do_send(osjob_t* j){
//     // Check if there is not a current TX/RX job running
//     if (LMIC.opmode & OP_TXRXPEND) {
//         Serial.println(F("OP_TXRXPEND, not sending"));
//     } else {
      
//         // Prepare upstream data transmission at the next possible time.
//         LMIC_setTxData2(1, mydata, sizeof(mydata)-1, 0);
//         Serial.println(F("Packet queued---"));
//         lora();
//     }
//     // Next TX is scheduled after TX_COMPLETE event.
// }

void setup() {
  Serial.begin(9600);
  Serial.println("Starting");


  #ifdef VCC_ENABLE
  // For Pinoccio Scout boards
  pinMode(VCC_ENABLE, OUTPUT);
  digitalWrite(VCC_ENABLE, HIGH);
  delay(1000);
  #endif

  // LMIC init
  os_init();
  //   // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();
  LoRa.setPins(5,15,2);

  //   // Start job (sending automatically starts OTAA too)
  do_send(&sendjob);

  // if (!LoRa.begin(868E6)) {
  //   Serial.println("Starting LoRa failed!");
  //   while (1);
  // }
}

void lora(){


  int packetSize = LoRa.parsePacket();
  // Serial.println(packetSize+3);
  // while(!packetSize){
  // if (packetSize) {
    // Serial.println(String(LoRa.packetRssi()));
    // os_runloop_once();
    // }
  // }
  while(!packetSize){
    packetSize = LoRa.parsePacket();
  }
  Serial.println("Recd");
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(String(LoRa.packetRssi()));

  }
  else{
    // Serial.print(".");
  }

  // LoRa.end();
}

void loop() {
  os_runloop_once();
  // lora();
  // rssi();
    
  
}
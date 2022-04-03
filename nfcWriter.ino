#include "ST25DVSensor.h"

#define SerialPort Serial
#define SerialPort1 Serial1

#if defined(ARDUINO_B_L4S5I_IOT01A)
// Pin definitions for board B-L4S5I_IOT01A
#define GPO_PIN PE4
#define LPD_PIN PE2
#define SDA_PIN PB11
#define SCL_PIN PB10

#define WireNFC MyWire
TwoWire MyWire(SDA_PIN, SCL_PIN);

#else
// Please define the pin and wire instance used for your board
#define GPO_PIN A1
#define LPD_PIN A2
#define SDA_PIN D4
#define SCL_PIN D5

#define WireNFC Wire // Default wire instance

#endif

#if !defined(GPO_PIN) || !defined(LPD_PIN)
#error define the pin and wire instance used for your board
#endif

String serialMsg = "";
char writeMsg[6] = "write";
// char writeMsg = "write";
// int incomingByte = 0;
// char inputString[100];
// char inputStringCharIndex = 0;
// bool stringComplete = false;

void setup()
{
  // Initialize serial for output.
  SerialPort.begin(115200);
  // Initialize on board serial tx rx lines
  SerialPort1.begin(115200);

  const char uri_write_message[] = "web-pager-backend.herokuapp.com"; // Uri message to write in the tag
  // const char uri_write_protocol[] = URI_ID_0x01_STRING;
  const char uri_write_protocol[] = "https://"; // Uri protocol to write in the tag
  String uri_write = String(uri_write_protocol) + String(uri_write_message);
  // String uri_write = "https://web-pager-backend.herokuapp.com/";
  String uri_read;

  pinMode(13, OUTPUT);

  while (!SerialPort)
  {
    delay(10);
  }

  // The wire instance used can be omited in case you use default Wire instance
  if (st25dv.begin(GPO_PIN, LPD_PIN, &WireNFC) == 0)
  {
    SerialPort.println("System Init done!");
    SerialPort1.println("System Init done!");
  }
  else
  {
    SerialPort.println("System Init failed!");
    SerialPort1.println("System Init failed!");
    while (1)
      ;
  }

  if (st25dv.writeURI(uri_write_protocol, uri_write_message, ""))
  {
    SerialPort.println("Write failed!");
    while (1)
      ;
  }

  // Wait before reading
  delay(100);

  if (st25dv.readURI(&uri_read))
  {
    SerialPort.println("Read failed!");
    while (1)
      ;
  }

  SerialPort.println(uri_read.c_str());

  if (strcmp(uri_read.c_str(), uri_write.c_str()) == 0)
  {
    SerialPort.println(uri_read.c_str());
    SerialPort.println("Successfully written and read!");
    digitalWrite(13, HIGH); // turn the LED on
  }
  else
  {
    SerialPort.println("Read bad string!");
  }

  delay(100);
}

void loop()
{
  // USB Connection or on board TX / RX lines.
  if (SerialPort.available() || SerialPort1.available())
  {
    if (SerialPort.available())
    {
      serialMsg = SerialPort.readStringUntil('\n');
    }
    else
    {
      serialMsg = SerialPort1.readStringUntil('\n');
    }

    // SerialPort.println(serialMsg);

    char arr[serialMsg.length() + 1];

    strcpy(arr, serialMsg.c_str());

    // SerialPort.println(arr);

    char *token = strtok(arr, ",");
    char *url = strtok(NULL, ",");

    // SerialPort.println(token);
    // SerialPort.println(writeMsg);

    if (strcmp(token, writeMsg) == 0)
    {
      SerialPort.println(url);
      updateNFCTag(url);
    }
  }

  // Wait
  delay(1);
}

bool updateNFCTag(char url[])
{
  bool sucuss = false;
  const char uri_write_protocol[] = "https://"; // Uri protocol to write in the tag
  String uri_write = String(uri_write_protocol) + String(url);
  // String uri_write = "https://web-pager-backend.herokuapp.com/";
  String uri_read;

  digitalWrite(13, LOW);

  // The wire instance used can be omited in case you use default Wire instance
  if (st25dv.begin(GPO_PIN, LPD_PIN, &WireNFC) == 0)
  {
    SerialPort.println("System Init done!");
    SerialPort1.println("System Init done!");
  }
  else
  {
    SerialPort.println("System Init failed!");
    SerialPort1.println("System Init failed!");
    while (1)
      ;
  }

  if (st25dv.writeURI(uri_write_protocol, url, ""))
  {
    SerialPort.println("Write failed!");
    SerialPort1.println("Write failed!");
    while (1)
      ;
  }

  // Wait before reading
  delay(100);

  if (st25dv.readURI(&uri_read))
  {
    SerialPort.println("Read failed!");
    SerialPort1.println("Read failed!");
    while (1)
      ;
  }

  SerialPort.println(uri_read.c_str());

  if (strcmp(uri_read.c_str(), uri_write.c_str()) == 0)
  {
    SerialPort.println(uri_read.c_str());
    SerialPort.println("Successfully written and read!");
    SerialPort1.println(uri_read.c_str());
    SerialPort1.println("Successfully written and read!");
    digitalWrite(13, HIGH); // turn the LED on
    sucuss = true;
  }
  else
  {
    SerialPort.println("Read bad string!");
    SerialPort1.println("Read bad string!");
  }

  delay(100);

  return sucuss;
}
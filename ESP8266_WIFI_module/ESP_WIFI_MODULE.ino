/*-------------------------- Includes -----------------------------*/
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
 
/*-------------------------- Macro declarations -----------------------------*/
// commands 
#define GET_VERSION       0x01
#define GET_CRC_FIEL      0x02
#define GET_SIZE_FILE     0x03
#define DOWNLOAD_APP      0x04

// CRC-32 parameters
#define CRC32_POLYNOMIAL 0xEDB88320L
// Define the packet size
#define PACKET_SIZE 256
/*-------------------------- Variable definations  -----------------------------*/
const char* ssid = "Youssef";
const char* password = "m8m28975";
const unsigned long commandDelay = 2000;  // 2 seconds
const char* serverAddress = "fotaserver.freevar.com";
const String versionURL = "/firmware/version.txt";
String version = "";  // Variable to store the firmware version
String serverURL = String(serverAddress);
String crc_server = ""; // string to save the crc from server in 
String file_size = "";  // string to save the file size from server in 
String AppURL = "";     // string to save the Application URL
size_t AppSize;         // variable to save the application size for dynamic heap allocation 
uint8_t* AppBuffer;     // uint8_t pointer to save the address of the dynamic heap allocation of the received app from server 

const int uartBaudRate = 115200;
const int connectionStatusLED = 2;  // GPIO2 for connection status LED

SoftwareSerial uart(3, 2);  // Define your RX and TX pins for UART communication

// Table for CRC-32 values
static uint32_t crc32_table[256];

/*-------------------------- Function Definations -----------------------------*/
/*                  initialization functions                    */
void setup() {
  pinMode(connectionStatusLED, OUTPUT);
  digitalWrite(connectionStatusLED, LOW);

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  waitForWiFiConnection();
  uart.begin(uartBaudRate);

  init_crc32_table();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    // Wi-Fi connection lost, attempt to reconnect
    waitForWiFiConnection();
 	}

	digitalWrite(connectionStatusLED, HIGH);
	// Check for commands from Serial and enough time has passed since the last command
	int receivedCommand = ReceiveCommand();

  if (receivedCommand != -1) {
    // Process the received command
    ProcessCommand(receivedCommand);
  }

}

void waitForWiFiConnection() {
  uint8_t timeDelay = 0;
  while (WiFi.status() != WL_CONNECTED) {

    WiFi.begin(ssid, password);  // Reattempt connection
    // Wait for 10 seconds before checking again
    while(timeDelay < 10)
    {
    	BlinkLED(500);
    	timeDelay++;
    }
  }

}

void BlinkLED(uint16_t period)
{
	  digitalWrite(connectionStatusLED, LOW);
    delay(period);
    digitalWrite(connectionStatusLED, HIGH);
    delay(period);
}						
int ReceiveCommand() {
  unsigned long startTime = millis();  // Record the start time
  String commandString = "";  // Variable to store the received command

  while (millis() - startTime < 1000) {  // Wait for up to one second
    if (Serial.available() > 0) {  // Check if there's data available to read
      char receivedChar = Serial.read();  // Read the character
      if (isdigit(receivedChar)) {
        commandString += receivedChar;  // Append the digit to the command string
      } else if (receivedChar == '\n') {
        // End of command received, convert the string to an integer
        return commandString.toInt();
      }
    }
  }

  // No valid command received within one second
  return -1;
}

void GetVersion() {
  HTTPClient http;
  WiFiClient client; // Create a WiFiClient instance
  http.begin(client, serverAddress, 80, versionURL); // Use the WiFiClient
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    version = http.getString();
    http.end();
    
    if (!(version.length() > 0)) {
      version = "0.0.0";
    }
  }
}

void GetCRC() {
  HTTPClient http;

  String CRC_URL = "http://" + serverURL + "@" + serverURL + "/firmware/" + version + "/crc.txt";
  //Serial.println(CRC_URL);

  WiFiClient client;
  http.begin(client, CRC_URL);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    crc_server = http.getString();
    //Serial.println(crc_server);
    http.end();
  } 
}

void GetFileSize() {
  HTTPClient http;

  String Size_URL = "http://" + serverURL + "@" + serverURL + "/firmware/" + version + "/size.txt";
  //Serial.println(Size_URL);

  WiFiClient client;
  http.begin(client, Size_URL);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    file_size = http.getString();
    http.end();
  } 
}

uint8_t* DownloadApp(const String& url, size_t& bufferSize) {
  HTTPClient http;
  WiFiClient client; // Create a WiFiClient instance

  // Use the WiFiClient with the HTTPClient
  http.begin(client, url);

  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    // Get the content length from the response headers
    int contentLength = http.getSize();
    if (contentLength <= 0) {
      http.end();
      return nullptr;
    }

    bufferSize = contentLength;
    uint8_t* buffer = new uint8_t[bufferSize];

    size_t bytesRead = 0;
    WiFiClient stream = http.getStream();
    while (stream.available()) {
      size_t chunkSize = stream.available();
      if (chunkSize > (bufferSize - bytesRead)) {
        chunkSize = (bufferSize - bytesRead);
      }
      bytesRead += stream.readBytes(buffer + bytesRead, chunkSize);
      chunkSize = stream.available();
    }
    http.end();

    if (bytesRead != contentLength) {
      // Downloaded content length doesn't match the expected length
      delete[] buffer;
      return nullptr;
    }

    return buffer;
  }

  return nullptr;
}

void ProcessCommand(uint8_t command) {
  // Process the received command
  //Serial.println("Received command: " + command);

  // Switch based on the command
  switch(command)
  {
  	case GET_VERSION:
      Serial.println("ACK");
  		BlinkLED(250);
  		GetVersion();
  		BlinkLED(250);
  		Serial.println(version);
  	break;

  	case GET_SIZE_FILE:
      Serial.println("ACK");
  		BlinkLED(250);
  		GetFileSize();
  		BlinkLED(250);
  		Serial.println(file_size);
  	break;

  	case GET_CRC_FIEL:
      Serial.println("ACK");		
      BlinkLED(250);		
  		GetCRC();
  		BlinkLED(250);
  		Serial.println(crc_server);
  	break;

  	case DOWNLOAD_APP:
      Serial.println("ACK");		  
      BlinkLED(250);
		  AppURL = "http://" + serverURL + "@" + serverURL + "/firmware/" + version + "/app.bin";
  		AppBuffer = DownloadApp(AppURL,AppSize);
  		BlinkLED(250);

  		if (AppBuffer != nullptr) {
		  // File downloaded successfully into the fileBuffer array
		  // now process the content in the array
		  uint32_t calculated_CRC;
		  calculated_CRC = calculate_crc32(AppBuffer,AppSize);
		  GetCRC();
		  if(calculated_CRC == (crc_server.toInt()))
		  {
		  	sendLargeDataViaUART(AppBuffer,AppSize);
		  }
		  // delete the buffer when you're done with it
		  delete[] AppBuffer;
		}
  	break;
    
    default:
    Serial.println("NACK");
    break;

  }
}

// Initialize the CRC-32 table
void init_crc32_table() {
    for (int i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (CRC32_POLYNOMIAL & (-(int32_t)(crc & 1)));
        }
        crc32_table[i] = crc;
    }
}

// Calculate CRC-32 for a given data buffer
uint32_t calculate_crc32(const uint8_t* data, size_t length) {
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < length; i++) {
        crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ data[i]];
    }
    return crc ^ 0xFFFFFFFF;
}

void sendLargeDataViaUART(uint8_t* data, size_t length) {
  // Calculate the number of packets
  size_t totalPackets = (length + PACKET_SIZE - 1) / PACKET_SIZE;

  for (size_t packetNum = 0; packetNum < totalPackets; packetNum++) {
    size_t startIdx = packetNum * PACKET_SIZE;
    size_t endIdx = min(startIdx + PACKET_SIZE, length);
    size_t bytesToSend = endIdx - startIdx;

    // Send the current packet via UART
    uart.write(data + startIdx, bytesToSend);

    // Delay for some time before sending the next packet
    delay(200);  // Adjust the delay time according to the receiving end's processing requirements
  }
}


#include <esp_now.h>
#include <cmath>
#include <HardwareSerial.h>
#include <WiFi.h>


// REPLACE WITH THE RECEIVER'S MAC Address
uint8_t broadcastAddress[] = {0xEC, 0x62, 0x60, 0x5A, 0x51, 0x88};

typedef struct struct_message {
    int id; // must be unique for each sender board
    int tempe;
    int humid;
    int occup;
    int lux;
}
        struct_message;

// Create a struct_message called myData
struct_message myData;
// Create peer interface
esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");}

// Potentiometer is connected to GPIO 34 (Analog ADC1_CH6)
const int pot1Pin = 34;
const int pot2Pin = 35;


// variable for storing the potentiometer value
int potValue1 = 0;
int potValue2 = 0;

const int tempPin = A0;     //analog input pin constant
int tempVal;    // temperature sensor raw readings
float volts;    // variable for storing voltage
float tempTMP;     // actual temperature variable


void setup() {
    Serial.begin(115200);
    delay(1000);
    pinMode(25, OUTPUT);
    digitalWrite(25, HIGH);
    pinMode(26, OUTPUT);
    digitalWrite(26, HIGH);



    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);

    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Once ESPNow is successfully Init, we will register for Send CB to
    // get the status of Trasnmitted packet
    esp_now_register_send_cb(OnDataSent);

    // Register peer
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    // Add peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
    }


}

void loop() {
    // Reading potentiometer value
    potValue1 = analogRead(pot1Pin);
    // Serial.print("Rita owes: ");
    // Serial.println(potValue1);
    // Serial.print("euros");
    //  Serial.print("\n");
    potValue2 = analogRead(pot2Pin);
    //  Serial.print("Analog value: ");
    //  Serial.println(potValue2);

    tempVal = analogRead(tempPin);
    volts = tempVal/1023.0;             // normalize by the maximum temperature raw reading range
    tempTMP = (volts - 0.5) * 100 ;
    delay(1000);





    // Set values to send
    myData.id = 4;
    myData.occup = potValue1 ;
    myData.lux = potValue2;
    myData.humid=0;
    myData.tempe=static_cast<int>(floor(tempTMP));

    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

    if (result == ESP_OK) {
        Serial.println("Sent with success");
    }
    else {
        Serial.println("Error sending the data");
    }
    delay(1000);

}
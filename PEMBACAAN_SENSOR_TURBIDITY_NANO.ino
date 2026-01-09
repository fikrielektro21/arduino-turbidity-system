// Arduino Nano - Sensor Node
#include <SoftwareSerial.h>
#define TURBIDITY_PIN A0


const int TURBIDITY_THRESHOLD = 500;  // Tresshold 200 = 1,0001 volt, treshold 500 = 2,444 volt.
SoftwareSerial rs485Serial(11, 10); // 10 pin rx, 11 tx

void setup() {
  Serial.begin(9600);     // Serial ke RS-485

}

void loop() {
  // logika 
  int kekeruhan = bacaKekeruhanStabil();
  int tegangan_mV = (kekeruhan * 5000L) / 1024;
  // pembacaan turbiditi dan tegangan 
  Serial.print("Turbidity = ");
  Serial.print(kekeruhan);
  Serial.print(" | Tegangan = ");
  Serial.print(tegangan_mV);
  Serial.println(" mV");

  // Kirim data via RS-485
  Serial.println(kekeruhan);
  rs485Serial.println(kekeruhan);

  delay(1000); // Sesuaikan kecepatan update
}

int bacaKekeruhanStabil() {
  int minVal = 1023;
  for (int i = 0; i < 20; i++) {       // Baca 20x selama ~2 detik
    int val = analogRead(TURBIDITY_PIN);
    if (val < minVal) minVal = val;    // Simpan nilai paling keruh (terendah)
    delay(100);
  }
  return minVal;
}

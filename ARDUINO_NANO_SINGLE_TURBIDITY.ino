#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define RELAY_PIN 5
#define BACA_ADC A7

// konstata maxx untuk nilai
const int BATAS_SANGAT_KERUH = 500;
const int BATAS_MIN_ADA_AIR = 600;
const int BATAS_JERNIH = 700;


const unsigned long DURASI_KONFIRMASI = 2000;
const unsigned long DURASI_PENGURASAN = 150000;


LiquidCrystal_I2C lcd(0x27, 16, 2);

enum Status {
  MENUNGGU,
  KONFIRMASI,
  MENGURAS,
  SELESAI
};
Status status = MENUNGGU;
unsigned long timer = 0;
bool relayNyala = false;
int bacaStabil() {
  int total = 0;
  for (int i = 0; i < 10; i++) {
    total += analogRead(BACA_ADC);
    delay(5);
  }
  return total / 10;
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  lcd.init();
  lcd.backlight();
  lcd.print("Tunggu...");
}

void loop() {
  int turbidity = bacaStabil();
  
  // tampikan di lcd
  Serial.print("ADC = ");
  Serial.println(turbidity);
  Serial.print(" | Status: ");

  if (status == MENUNGGU)       Serial.print("MENUNGGU");
  else if (status == KONFIRMASI) Serial.print("KONFIRMASI");
  else if (status == MENGURAS)   Serial.print("MENGURAS");
  else if (status == SELESAI)    Serial.print("SELESAI");

  Serial.print(" | Relay: ");
  Serial.print(relayNyala ? "ON" : "OFF");
  
  unsigned long sekarang = millis();
  // state machine
  switch (status) {
    case MENUNGGU:
      if (turbidity > BATAS_JERNIH && turbidity > BATAS_MIN_ADA_AIR) {
        status = KONFIRMASI;
        timer = sekarang;
      }
      break;
    case KONFIRMASI:
      if (turbidity > BATAS_JERNIH && turbidity > BATAS_MIN_ADA_AIR) {
        if (sekarang - timer >= DURASI_KONFIRMASI) {
          digitalWrite(RELAY_PIN, LOW);
          relayNyala = true;
          status = MENGURAS;
          timer = sekarang;
        }
      } else {
        status = MENUNGGU;
      }
      break;
    case MENGURAS:
      if (sekarang - timer >= DURASI_PENGURASAN) {
        digitalWrite(RELAY_PIN, HIGH);
        relayNyala = false;
        status = SELESAI;
        timer = sekarang;
      }
      break;
    case SELESAI:
      if (sekarang - timer >= 1000) {
        status = MENUNGGU;
      }
      break;
  }

  lcd.setCursor(0, 0);
  lcd.print("KEKERUHAN :     ");
  lcd.setCursor(11, 0);
  lcd.print(turbidity);

  lcd.setCursor(0, 1);
  lcd.setCursor(0, 1);
  Serial.print("DEBUG LCD: turbidity = ");
  Serial.println(turbidity);
  if (status == MENGURAS) {
    unsigned long sisa = (DURASI_PENGURASAN - (sekarang - timer)) / 1000;
    lcd.print("MENGURAS:");
    lcd.print(sisa);
    lcd.print("S");
  } else if (status == KONFIRMASI) {
    lcd.print("Tunggu...       ");
  } else if (turbidity <= BATAS_SANGAT_KERUH) {
    lcd.print("AIR KOTOR       ");
  } else if (turbidity >= 600 && turbidity <= 699) {
    lcd.print("TIDAK ADA AIR   ");
  } else if (turbidity >= 700) {
    lcd.print("AIR JERNIH      ");
  } else {
    lcd.print("POMPA: OFF      ");
  }

  delay(100);
}




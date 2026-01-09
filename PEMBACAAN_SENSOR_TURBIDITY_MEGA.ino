// Arduino Mega - RS-485 + LCD I2C + Relay (Pengurasan 5 Menit)
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define RELAY_PIN 7

// Ambang batas berdasarkan pengukuranmu
const int BATAS_SANGAT_KERUH = 500;
const int BATAS_MIN_ADA_AIR  = 690;
const int BATAS_JERNIH       = 630;

// Durasi (dalam milidetik)
const unsigned long DURASI_KONFIRMASI = 2000;    // 2 detik
const unsigned long DURASI_PENGURASAN = 180000;   // 5 menit = 300.000 ms

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Status sistem
enum StatusSistem {
  MENUNGGU_JERNIH,
  MENUNGGU_KONFIRMASI,
  PENGURASAN_AKTIF,
  POMPA_MATI
};

StatusSistem status = MENUNGGU_JERNIH;
unsigned long timer_mulai = 0;
bool relay_nyala = false;

void matikanPompa() {
  relay_nyala = false;
  digitalWrite(RELAY_PIN, HIGH); // active LOW
}

void nyalakanPompa() {
  relay_nyala = true;
  digitalWrite(RELAY_PIN, LOW);
}

void setup() {
  Serial1.begin(9600);
  Serial.begin(115200);
  
  pinMode(RELAY_PIN, OUTPUT);
  matikanPompa(); // Pastikan mati

  lcd.init();
  lcd.backlight();
  lcd.print("Sistem Siap");
  delay(1000);
  lcd.clear();
}

void loop() {
  static int turbidity = 600; // Nilai default

  // Baca data dari Nano (jika tersedia)
  if (Serial1.available()) {
    String data = Serial1.readStringUntil('\n');
    data.trim();
    if (data.length() > 0 && isDigit(data[0])) {
      turbidity = data.toInt();
    }
  }

  unsigned long waktu_sekarang = millis();

  // === STATE MACHINE ===
  switch (status) {
    case MENUNGGU_JERNIH:
      if (turbidity > BATAS_JERNIH && turbidity > BATAS_MIN_ADA_AIR) {
        status = MENUNGGU_KONFIRMASI;
        timer_mulai = waktu_sekarang;
      }
      break;

    case MENUNGGU_KONFIRMASI:
      if (turbidity > BATAS_JERNIH && turbidity > BATAS_MIN_ADA_AIR) {
        if (waktu_sekarang - timer_mulai >= DURASI_KONFIRMASI) {
          // Konfirmasi sukses → mulai pengurasan
          nyalakanPompa();
          status = PENGURASAN_AKTIF;
          timer_mulai = waktu_sekarang; // Reset timer untuk 5 menit
        }
      } else {
        // Kondisi jernih hilang → kembali ke awal
        status = MENUNGGU_JERNIH;
      }
      break;

    case PENGURASAN_AKTIF:
      // Abaikan sensor! Pompa tetap nyala selama 5 menit
      if (waktu_sekarang - timer_mulai >= DURASI_PENGURASAN) {
        matikanPompa();
        status = POMPA_MATI;
        timer_mulai = waktu_sekarang; // Untuk delay sebelum mulai deteksi ulang
      }
      break;

    case POMPA_MATI:
      // Tunggu 1 detik sebelum kembali deteksi (opsional)
      if (waktu_sekarang - timer_mulai >= 1000) {
        status = MENUNGGU_JERNIH;
      }
      break;
  }

  // === TAMPILAN LCD ===
  lcd.setCursor(0, 0);
  lcd.print("Kekeruhan:      ");
  lcd.setCursor(11, 0);
  lcd.print(turbidity);

  lcd.setCursor(0, 1);
  if (status == PENGURASAN_AKTIF) {
    unsigned long sisa = (DURASI_PENGURASAN - (waktu_sekarang - timer_mulai)) / 1000;
    lcd.print("MENGGURASI:");
    if (sisa > 999) sisa = 999;
    lcd.print(sisa);
    lcd.print("s ");
  } else if (status == MENUNGGU_KONFIRMASI) {
    lcd.print("Tunggu...       ");
  } else if (turbidity <= BATAS_SANGAT_KERUH) {
    lcd.print("AIR KOTOR       ");
  } else if (turbidity <= BATAS_MIN_ADA_AIR) {
    lcd.print("BAK KOSONG   ");
  } else if (turbidity <= BATAS_JERNIH) {
    lcd.print("BAK KOSONG       ");
  } else {
    lcd.print("POMPA: OFF      ");
  }

  // Debug
  Serial.print("Turbidity: ");
  Serial.print(turbidity);
  Serial.print(" | Status: ");
  if (status == MENUNGGU_JERNIH) Serial.print("MENUNGGU");
  else if (status == MENUNGGU_KONFIRMASI) Serial.print("KONFIRMASI");
  else if (status == PENGURASAN_AKTIF) Serial.print("PENGURASAN");
  else Serial.print("MATI");
  Serial.print(" | Relay: ");
  Serial.println(relay_nyala ? "ON" : "OFF");

  delay(100);
}
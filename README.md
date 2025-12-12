# Arduino Turbidity-Based Water Drainage System

Automatically drains **clear water** from a storage tank based on real-time turbidity sensor readings. Designed for field use in household water management systems.

## 🌟 Features
- ✅ **2-second confirmation** before pump activation (avoids false triggers)
- ⏱️ **5-minute automatic drainage cycle** once clear water is confirmed
- 🚫 **Dry-run protection**: Pump **will not turn on** if sensor is not submerged
- 📊 **Real-time LCD display** (16x2 I2C) showing:
  - Turbidity value (ADC)
  - System status: `AIR KOTOR`, `TIDAK ADA AIR`, `AIR JERNIH`, `MENGURAS:XXs`
- 🔌 **Single Arduino Nano** solution (all-in-one: sensor, relay, LCD)

## 🧰 Hardware Used
- **Microcontroller**: Arduino Nano
- **Sensor**: Analog Turbidity Sensor (e.g., SEN0189 or compatible)
- **Actuator**: 5V Relay Module + opto pc817 (active LOW)
- **Display**: LCD 16x2 with I2C Backpack (address `0x27` or `0x3F`)
- **Power**: 5V regulated supply (e.g., LM2596 from 12V SMPS)

## ⚙️ Key Logic
- **Air jernih**: ADC ≥ 700 → after 2s confirmation → pump ON for 5 minutes
- **Tidak ada air**: 600 ≤ ADC ≤ 699 → pump OFF
- **Air kotor**: ADC ≤ 500 → pump OFF
- Pump **ignores sensor readings during 5-minute drainage** (ensures full cycle even if water level drops)

## 📝 Lessons Learned
- ❌ **Avoid long analog cables** (>1m) in industrial/noisy environments → causes unstable readings
- ✅ **Always calibrate thresholds on-site** with actual cabling installed
- 💧 **Waterproof the sensor node** if deployed near open tanks
- 🔁 **Use integer math & non-blocking delays** for reliable real-time control

## 📂 File Structure
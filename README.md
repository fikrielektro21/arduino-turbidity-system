# Arduino NANO & MEGA 
This is for the case of a tank that is far from the main panel, I use an Arduino Mega on the main panel with additional RS485 Communication to read and send data. The Arduino Mega's task is to display the read turbidity value sent via RS485 communication and also to control the relay connected to the AC pump. Remember to use a relay + opocoupler / optoisolator to protect the Arduino from starting surges in the AC pump. The Arduino Nano's task is to read the turbidity sensor using (AnalogRead) and send it to the Arduino Mega via RS485. Since the Arduino Nano only has 1 UART register, I use softwareserial to use RS485 communication on the NANO side.

![Schematic](circuit_image(4).png)

# Arduino NANO Turbidity

Automatically drains **clear water** from a storage tank based on real-time turbidity sensor readings. Designed for field use in household water management systems. 
In this project, the need for a clear water filter or what will be discarded is clear water. Therefore, if you want to use this source, you must change the state machine section "MENGURASI" change the logic to "<" for the operation to clear water.
And the determination of the numbers in the variables is based on the results of measurements in my lab, not in the field, so there is a possibility of changing in the field. Yes, because I did not calibrate to 0, I only read the ADC value directly by comparing it with the desired water conditions. This does seem a bit confusing, but this is a method if you do not have fluid for calibration.
**Remember!!!** This method is not to be imitated in the scientific writing process because there is no calibration here.

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
- **Sensor**: Analog Turbidity Sensor (e.g., SEN0189 or compatible) https://share.google/KoyhsTsTZ1xjH7zKy
- **Actuator**: 5V Relay Module + opto pc817 (active LOW) https://share.google/MdFoHZ9gB2fQCH2jA
- **Display**: LCD 16x2 with I2C Backpack (address `0x27` or `0x3F`) https://share.google/1XS4DpJi7etgQmaFK
- **Power**: 5V regulated supply (e.g., LM2596 from 12V SMPS) https://share.google/D3ReLLn5qp7iZkWeH | https://share.google/2aIWy9NJLImbFmbCs

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

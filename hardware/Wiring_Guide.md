# AERO-SONAR — Hardware Master Wiring Guide

**Smart India Hackathon 2026 — Problem Statement 26058**  
**Ministry of Earth Sciences (MoES) / National Institute of Ocean Technology (NIOT)**  
**Team AEROS (Team ID: 186870)**  

---

## 1. Overview

This document specifies the pinout and wiring connections for the **STM32G474RE Nucleo-64** development platform operating in **8-Sensor Hardware Emulation Mode** and interfacing with the analog transmitter signal chain.

The 8 potentiometers ($10\,\text{k}\Omega$ linear, $3.3\,\text{V}$ voltage dividers) emulate:
- **Group 1: PS 26058 Core Mandated Inputs (4):** Turbidity, Depth, Temperature, Salinity.
- **Group 2: Team Acoustic Innovations (+4):** Battery State, Target Range, Vehicle Speed, Ambient Noise.

---

## 2. Master Pin Mapping (STM32G474RE)

| Pin | Arduino Pin | Peripheral / Function | Connected Device | Description / Range |
|---|---|---|---|---|
| **PA0** | A0 | `ADC1_IN1` | POT 1: Turbidity | Emulates water turbidity ($0 - 100\,\text{NTU}$) |
| **PA1** | A1 | `ADC1_IN2` | POT 2: Water Depth | Emulates vehicle depth ($0 - 200\,\text{m}$) |
| **PA4** | A2 | `DAC1_OUT1` | Analog Signal Chain | 12-bit analog chirp / pulse output |
| **PB0** | A3 | `ADC1_IN15` | POT 3: Temperature | Emulates water temperature ($0 - 30\,^\circ\text{C}$) |
| **PC0** | A4 | `ADC1_IN6` | POT 4: Salinity | Emulates salinity ($25 - 38\,\text{ppt}$) |
| **PC1** | A5 | `ADC1_IN7` | POT 5: Battery State | Emulates vehicle battery level ($0 - 100\%$) |
| **PC2** | D0 | `ADC1_IN8` | POT 6: Target Range | Emulates target detection distance ($10 - 200\,\text{m}$) |
| **PC3** | D1 | `ADC1_IN9` | POT 7: Vehicle Speed | Emulates AUV forward velocity ($0.0 - 3.0\,\text{m/s}$) |
| **PC4** | D2 | `ADC2_IN5` | POT 8: Ambient Noise | Emulates ocean ambient noise level ($30 - 80\,\text{dB}$) |
| **PB5** | D4 | `GPIO_Output` | LC Relay 1 Control | Switched LC Inductor Bank Selection |
| **PB4** | D5 | `GPIO_Output` | LC Relay 2 Control | Switched LC Capacitor Bank Selection |
| **PB10** | D6 | `TIM2_CH3 / PWM` | Class-D PA Enable | High-efficiency amplifier burst gating |
| **PB8** | D15 | `I2C1_SCL` | INA226 / INA219 | Current Shunt Sensor Clock (Energy Monitoring) |
| **PB9** | D14 | `I2C1_SDA` | INA226 / INA219 | Current Shunt Sensor Data (Energy Monitoring) |
| **PA2** | D1 | `USART2_TX` | ST-LINK VCP / USB | 115200 baud JSON Telemetry Stream to Web UI |
| **PA3** | D0 | `USART2_RX` | ST-LINK VCP / USB | Serial Command Ingestion |
| **PA5** | D13 | `GPIO_Output` | User Green LED | Heartbeat & ping transmission flash |

---

## 3. Potentiometer Wiring Scheme

All eight $10\,\text{k}\Omega$ potentiometers are wired identically as precision voltage dividers:

```text
                 +3.3V (VREF+)
                     │
               ┌─────┴─────┐
               │  10 kΩ    │ (Terminal 1)
               │  POT      │
               │  WIPER    ├───────────► STM32 ADC Analog Input Pin
               │           │ (Terminal 2)
               └─────┬─────┘
                     │
                    GND
```

- **Outer Terminal 1:** Wired to common clean $+3.3\,\text{V}$ analog reference rail (`VDDA`).
- **Outer Terminal 2:** Wired to common ground plane (`AGND`).
- **Center Wiper Terminal:** Connects directly to the dedicated STM32 ADC pin, providing an analog voltage strictly in the range $0.0\,\text{V} - 3.3\,\text{V}$.

---

## 4. Analog Signal Chain & Matching Wiring

```text
STM32 PA4 (DAC1_OUT1)
        │
        ▼
┌─────────────────────────┐
│ Sallen-Key 4th-Order    │
│ Active Low-Pass Filter  │ (Cutoff fc = 160 kHz for Band 1 / 550 kHz for Band 2)
└───────────┬─────────────┘
            │
            ▼
┌─────────────────────────┐
│ Ultra-Low Distortion    │
│ Pre-Amp Buffer (AD8066) │
└───────────┬─────────────┘
            │
            ▼
┌─────────────────────────┐
│ High-Efficiency         │ ◄────── PB10 (PA Gate Burst Gating)
│ Class-D Power Amplifier │
└───────────┬─────────────┘
            │
            ▼
┌─────────────────────────┐
│ Switched LC Matching    │ ◄────── PB4, PB5 (Relay States toggled during PRI)
│ Network                 │
└───────────┬─────────────┘
            │
            ▼
    Underwater Transducer (Piezo Projector)
```

> **Grounding Best Practice:** Keep Analog Ground (`AGND`) and Power Ground (`PGND` from the Class-D stage) isolated with a single-point star ground bead directly at the power supply terminal to prevent switching ripple from coupling into sensitive sensor ADC lines.

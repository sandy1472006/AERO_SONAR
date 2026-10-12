# AERO-SONAR — Master Bill of Materials (BOM)

**Smart India Hackathon 2026 — Problem Statement 26058**  
**Ministry of Earth Sciences (MoES) / National Institute of Ocean Technology (NIOT)**  
**Team AEROS (Team ID: 186870)**  

---

## 1. Electronics Bill of Materials

| Item # | Component Category | Manufacturer & Part Number | Description / Specifications | Qty | Designator | Role in System |
|---|---|---|---|---:|---|---|
| **1** | Main Controller | STMicroelectronics `NUCLEO-G474RE` | ARM Cortex-M4F @ 170 MHz, Hardware CORDIC, FPU, 512 KB Flash, 128 KB SRAM | 1 | U1 | Master EWA Engine & Waveform Synth |
| **2** | Operational Amplifier | Analog Devices `AD8066ARZ` | Dual High-Speed Rail-to-Rail FET-Input Op-Amp (145 MHz BW, 180 V/µs slew) | 2 | U2, U3 | Sallen-Key Active Reconstruction Filter & Driver |
| **3** | Power Amplifier IC | Texas Instruments `TPA3116D2` | High-Efficiency Class-D Stereo/Mono Amp (50W, >90% efficiency, 4.5V–26V) | 1 | U4 | Acoustic Power Transmit Stage |
| **4** | Current Shunt Sensor | Texas Instruments `INA226AIDGSR` | High-Side Precision Current/Power Monitor, 16-bit I2C, 0.1% accuracy | 1 | U5 | Active Energy-per-Ping Feedback |
| **5** | Shunt Resistor | Vishay Dale `WSL2512R0500FEA` | $50\,\text{m}\Omega$, 1 W, 1% Metal Strip Kelvin Precision Current Shunt | 1 | R_SHUNT | PA Supply Current Sensing |
| **6** | Signal Relays | Omron `G6K-2F-Y DC5` | Low-Power Subminiature DPDT Telecom Latching Relay, 5V coil | 2 | K1, K2 | Switched LC Matching Network Configuration |
| **7** | Power Inductors | Würth Elektronik `74435571100` | Shielded Power Inductor $15\,\mu\text{H}$, $10\,\text{A}$, low ESR | 2 | L1, L2 | LC Impedance Matching Inductor Bank |
| **8** | RF Capacitors | KEMET `C1210C104J5GACTU` | $100\,\text{nF}$, 50 V, C0G/NP0 Low-Loss High-Q MLCC Capacitors | 4 | C1–C4 | LC Matching & Filter Tuning |
| **9** | Input Potentiometers | Bourns `3386P-1-103LF` | $10\,\text{k}\Omega$ Cermet 1-Turn Potentiometer (or Alpha $16\,\text{mm}$ rotary) | 8 | POT1–8 | 8-Sensor Environmental Emulation |
| **10** | Voltage Regulators | Texas Instruments `LM2596S-5.0` | Step-Down Buck Converter (5.0V / 3A) for relays and PA bias | 1 | U6 | DC-DC Power Distribution |

---

## 2. Transducer & Acoustic Payload Parts

| Item # | Category | Part Description | Specifications | Qty | Role |
|---|---|---|---:|---|
| **11** | Acoustic Transducer | PZT-5A Tonpilz Projector | Piezoelectric broadband transducer (Resonance $100\,\text{kHz}$, usable $70 - 140\,\text{kHz}$) | 1 | Primary Acoustic Emitter |
| **12** | Hydrophone (Optional) | Aquarian Audio `H1a` | Omnidirectional broadband hydrophone (Receive sensitivity $-200\,\text{dB}$ re $1\,\text{V}/\mu\text{Pa}$) | 1 | Closed-Loop Echo Receiver |
| **13** | Pressure Hull (Prototype) | Heavy-Duty PVC-U Pipe | Outer Diameter $90\,\text{mm}$, Length $200\,\text{mm}$, Wall $6\,\text{mm}$, with machined endcaps | 1 | Prototype Dry Electronics Housing |
| **14** | Endcap Seals | Parker Nitrile O-Rings | 70 Shore A Buna-N radial O-rings, dash size -153 | 4 | Water Sealing |
| **15** | Chassis Mounting | 3D Printed PETG Slide Tray | Custom conformal structural electronics mounting rack | 1 | Internal Structural Integration |

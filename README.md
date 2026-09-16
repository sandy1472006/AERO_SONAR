# AERO-SONAR — Interactive Sonar Adaptation Simulator

> **Smart India Hackathon 2026** · Problem Statement 26058  
> Ministry of Earth Sciences (MoES) / National Institute of Ocean Technology (NIOT)  
> **Tagline:** Sense • Decide • Shape • Save

A single-screen, browser-based interactive digital twin and bench-test harness of the **AERO-SONAR** transmitter system. It simulates real-time environmental acoustic adaptation across the **100 kHz – 500 kHz** frequency band, waveform synthesis (LFM chirp & HFM), Tukey windowing, real Radix-2 1024-point FFT spectrum analysis, and active safety corridors.

---

## 🌊 Core Features & Architecture

- **4 PS Mandated Core Sensors (Problem Statement 26058)**:
  - **Turbidity** (`0 – 100 NTU`): Governs acoustic sediment scattering ($\alpha_{\text{turb}}$) via Thorne & Hanes (2002); triggers low-frequency 100 kHz penetration when $>50\text{ NTU}$.
  - **Water Depth** (`0 – 200 m`): Directly scales acoustic link budget and transmission loss; high authority switching from 500 kHz in shallow water to 100 kHz in deep waters.
  - **Temperature** (`0 – 30 °C`): Evaluates sound speed via Mackenzie (1981) and thermal absorption loss slope.
  - **Salinity** (`25 – 38 ppt`): Computes $MgSO_4$ chemical relaxation absorption and estuarine freshwater dilution.

- **4 Team Acoustic Innovations (+4 Added)**:
  - **Battery State** (`0 – 100 %`): Features a visible dial danger sector and tag for `≤20% POWER-SAVE`. Automatically caps transmit amplitude to 35% and pulse duration to 0.60ms to preserve vehicle propulsion.
  - **Target Range** (`10 – 200 m`): Dynamic acoustic path demand scaling chirp duration and frequency.
  - **Vehicle Speed** (`0.0 – 3.0 m/s`): Calculates Doppler Index $D = \frac{2 \cdot v \cdot B \cdot \tau}{c}$, switching to scale-invariant Hyperbolic Frequency Modulated (HFM) chirps if $D > 2.0$.
  - **Ambient Noise** (`30 – 80 dB`): Dynamically adjusts transmission power via the active sonar equation.

- **100 kHz – 500 kHz Wideband Acoustic Operation**:
  - **HIGH-RES (450 – 500 kHz, 75 kHz bandwidth)**: Delivers razor-sharp **1.0 cm** range resolution ($\Delta R = \frac{c}{2B}$) for fine seabed feature imaging in clear shallow water.
  - **BALANCED (250 – 300 kHz, ~45 kHz bandwidth)**: Optimal coastal operating regime providing 2.0 cm resolution.
  - **LONG-RANGE (100 kHz, 15 kHz bandwidth)**: High-penetration low-frequency mode designed to penetrate muddy waters and deep water absorption ($>140\text{ dB/km}$ at 500 kHz vs $34\text{ dB/km}$ at 100 kHz).
  - **POWER-SAVE**: Clamped low-power 150 kHz pulse triggered when battery $\le 20\%$.
  - **SAFE-FALLBACK**: Robust 100 kHz failsafe during severe acoustic link stress.

- **DSP & Bench Instrumentation**:
  - **Oscilloscope (Time Domain)**: 1024-point real-time chirp synthesis at 1.5 MSPS showing instantaneous frequency sweep and Tukey(0.15) window cosine tapers.
  - **FFT Spectrum Analyzer (0 – 600 kHz Span)**: In-place Radix-2 Cooley-Tukey 1024-point FFT with calibrated dB scale (-80 dB to 0 dB) and $f_{\text{start}}$ / $f_{\text{stop}}$ boundary markers.
  - **Windowing Proof**: Interactive toggle demonstrating out-of-band sidelobe suppression jumping from -13.4 dB (rectangular) down to -52.1 dB (Tukey).
  - **AUV Oceanic Hero Canvas**: 60 FPS real-time visualization with dynamic light penetration, 200 suspended sediment particles, bathymetric seabed with shipwreck profile, and frequency/absorption-dependent ping arc extinction.
  - **WebSerial Hardware Link**: Direct USB connection to STM32G4 Nucleo boards at 115200 baud.

---

## 🚀 Live Demo

Access the live simulator:
👉 **[https://sandy1472006.github.io/AERO_SONAR/](https://sandy1472006.github.io/AERO_SONAR/)** *(hosted on GitHub Pages)*

---

## 🛠️ Offline Usage

This project is built as a single, self-contained `index.html` file with zero external dependencies, no build steps, and full offline capability.

Simply clone the repository and open `index.html` in any web browser:
```bash
git clone https://github.com/sandy1472006/AERO_SONAR.git
cd AERO_SONAR
# Open index.html directly in Chrome or Edge
```

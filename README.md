# AERO-SONAR — Interactive Sonar Adaptation Simulator

> **Smart India Hackathon 2026** · Problem Statement 26058  
> Ministry of Earth Sciences (MoES) / National Institute of Ocean Technology (NIOT)  
> **Tagline:** Sense • Decide • Shape • Save

A single-screen, browser-based interactive digital twin and bench-test harness of the **AERO-SONAR** transmitter system. It simulates real-time environmental acoustic adaptation, waveform synthesis (LFM chirp & HFM), Tukey windowing, real Radix-2 1024-point FFT spectrum analysis, and active safety corridors.

---

## 🌊 Core Features

- **8 Environmental Controls**: Depth, Temperature, Salinity, Ambient Noise, Target Range, Vehicle Speed, Battery State, and Turbidity.
- **AUV Oceanic Hero Canvas**: 60 FPS real-time visualization with dynamic light penetration, 200 suspended sediment particles, bathymetric seabed with shipwreck profile, and frequency/absorption-dependent ping arc extinction and seabed echo returns.
- **Oscilloscope (Time Domain)**: Real 1024-point chirp synthesis showing instantaneous frequency sweep and Tukey(0.15) window cosine tapers.
- **FFT Spectrum Analyzer**: In-place Radix-2 Cooley-Tukey 1024-point FFT with calibrated dB scale (-80 dB to 0 dB) and f_start / f_stop boundary markers.
- **Windowing Proof**: Interactive toggle demonstrating out-of-band sidelobe suppression jumping from -13.4 dB (rectangular) down to -52.1 dB (Tukey).
- **EWA Safety Corridors**: BALANCED (100 kHz), LONG-RANGE (70 kHz), HIGH-RES (140 kHz), POWER-SAVE (amber), and SAFE-FALLBACK (red).
- **Doppler Metric & HFM Fallback**: Continuous computation of $D = \frac{2 \cdot v \cdot B \cdot \tau}{c}$, switching to scale-invariant Hyperbolic Frequency Modulated (HFM) pulses if $D > 2.0$.
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

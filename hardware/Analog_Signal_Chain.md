# AERO-SONAR — Analog Signal Chain & Matching Network Design

**Smart India Hackathon 2026 — Problem Statement 26058**  
**Ministry of Earth Sciences (MoES) / National Institute of Ocean Technology (NIOT)**  
**Team AEROS (Team ID: 186870)**  

---

## 1. Analog Signal Chain Topology

A software-defined sonar transmitter requires an analog stage capable of converting digital DAC staircase samples into high-purity continuous-time acoustic signals, amplifying them efficiently, and transferring power to a reactive piezoelectric load without reflections:

```text
┌──────────────┐     ┌───────────────────────┐     ┌──────────────────────┐
│  STM32G4     │     │ Sallen-Key 4th-Order  │     │ Ultra-Low Distortion │
│  DAC1_OUT1   ├────►│ Butterworth LPF       ├────►│ Pre-Driver Buffer    │
│  (0 - 3.3V)  │     │ (fc = 160 kHz / 550k) │     │ (AD8066 Op-Amp)      │
└──────────────┘     └───────────────────────┘     └──────────┬───────────┘
                                                              │
                                                              ▼
┌──────────────┐     ┌───────────────────────┐     ┌──────────────────────┐
│ Piezo Project│     │ Switched LC Matching  │     │ Class-D Power Amp    │
│ Tonpilz PZT  │◄────┤ Conjugate Network     │◄────┤ (TPA3116D2 / >90%)   │
│ (Reactive)   │     │ (PRI Relay Toggled)   │     │ (Pulse Gated)        │
└──────────────┘     └───────────────────────┘     └──────────▲───────────┘
                                                              │
                                                   ┌──────────┴───────────┐
                                                   │ Shunt Current Sensor │
                                                   │ (INA226 - ∫ V·I dt)  │
                                                   └──────────────────────┘
```

---

## 2. Active Reconstruction Filter Design

The DAC staircase output produces high-frequency Nyquist sampling images at $f_{\text{image}} = f_s \pm f_{\text{signal}}$. For $f_s = 1.5\,\text{MSPS}$ and $f_{\text{signal}} = 140\,\text{kHz}$, images exist at $1.36\,\text{MHz}$ and $1.64\,\text{MHz}$.

To eliminate these spectral images:
- **Topology:** Two cascaded 2nd-order Sallen-Key active low-pass stages (4th-order Butterworth response).
- **Cutoff Frequency ($f_c$):** Designed with dual switched poles:
  - Band 1 (70–140 kHz mode): $f_c = 160\,\text{kHz}$, $-80\,\text{dB/decade}$ attenuation roll-off.
  - Band 2 (500 kHz mode): $f_c = 550\,\text{kHz}$.
- **Rejection:** Provides $>45\,\text{dB}$ attenuation at the nearest image frequencies, delivering clean sinusoidal waveforms into the power stage with Total Harmonic Distortion (THD) $< -42\,\text{dBc}$.

---

## 3. High-Efficiency Class-D Power Amplifier

Conventional Class-A or Class-AB linear power amplifiers exhibit poor theoretical efficiencies (typically $35\% - 55\%$), dissipating most battery energy as heat. In an enclosed AUV payload pod, this heat builds up rapidly and can cause thermal shutdown.

**AERO-SONAR's Class-D Approach:**
- Operates in switched mode (PWM bridge) with power MOSFETs switching between saturation and cutoff.
- **Power Efficiency:** Consistently exceeds **$88\% - 92\%$** under matched load conditions.
- **Dynamic Gate Shutdown:** The amplifier enable line is pulled LOW during the inter-ping quiet intervals (Pulse Repetition Interval, PRI), dropping idle current draw to $<1\,\mu\text{A}$.
- **Thermal Benefit:** Operates without massive external heat sinks, keeping internal pod temperature well within safe operating limits ($<40\,^\circ\text{C}$).

---

## 4. Switched LC Impedance Matching Network

A piezoelectric transducer exhibits an equivalent electrical circuit (Van Dyke model) comprising:
- $C_0$: Static clamped capacitance (typically $3\,\text{nF} - 8\,\text{nF}$).
- $R_m, L_m, C_m$: Motional branch components representing mechanical acoustic resonance.

At non-resonant frequencies, the transducer presents a large reactive capacitive impedance ($X_c = \frac{1}{2\pi f C_0}$), which reflects electrical power and causes high circulating currents.

```text
                  L_series             C_parallel
PA Output ───────(Inductor)──────┬──────(Capacitor)──────► Piezo Projector (+)
                                 │
                            [Relay K1/K2]
                                 │
                                GND
```

### Dynamic Tuning Rules:
1. **Low Frequency Mode (70 kHz – 100 kHz):** Relay K1 selects an additional series inductance ($L_{\text{match}} \approx 33\,\mu\text{H}$) to cancel the large capacitive reactance at lower frequencies.
2. **High Frequency Mode (140 kHz – 500 kHz):** Relay K1 switches to lower inductance ($L_{\text{match}} \approx 10\,\mu\text{H}$) and tunes parallel capacitance to maintain real impedance $Z_{\text{in}} \approx 50\,\Omega$.
3. **Switching Protocol:** Micro-relays switch **strictly during the zero-current PRI interval**, eliminating arcing, contact bounce noise, and transmission disruptions.

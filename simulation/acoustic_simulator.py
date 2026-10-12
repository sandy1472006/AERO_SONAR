#!/usr/bin/env python3
"""
========================================================================================
Project: AERO-SONAR : Adaptive Sonar Transmitter Payload
Module: Acoustic Propagation & Ocean Physics Simulator
Team: Team AEROS (Team ID: 186870) | SIH 2026 (Problem Statement 26058)
========================================================================================
Implements:
  1. Mackenzie (1981) 9-term empirical sound velocity equation c(T, S, D)
  2. Francois & Garrison (1982) dual chemical relaxation absorption model alpha(f, T, S, D, pH)
  3. Thorne & Hanes (2002) viscous & Rayleigh sediment attenuation alpha_sed(M, f)
  4. Urick Active Sonar Equation SNR = SL - 2TL + TS - (NL - DI)
  5. Doppler Sensitivity Index D = Bandwidth * Duration (B * T)
========================================================================================
"""

import math
import sys

def mackenzie_sound_speed(temp_c: float, salinity_ppt: float, depth_m: float) -> float:
    """
    Mackenzie (1981) empirical formula for sound speed in seawater (m/s).
    Valid for T: 2 to 30 C, S: 25 to 40 ppt, D: 0 to 8000 m.
    RMS error: 0.07 m/s.
    """
    T = temp_c
    S = salinity_ppt
    D = depth_m

    c = (1448.96 +
         4.591 * T -
         5.304e-2 * (T ** 2) +
         2.374e-4 * (T ** 3) +
         1.340 * (S - 35.0) +
         1.630e-2 * D +
         1.675e-7 * (D ** 2) -
         1.025e-2 * T * (S - 35.0) -
         7.139e-13 * T * (D ** 3))
    return c

def francois_garrison_absorption(freq_khz: float, temp_c: float, salinity_ppt: float,
                                 depth_m: float, ph: float = 8.1) -> float:
    """
    Francois & Garrison (1982) acoustic absorption coefficient in seawater (dB/km).
    Accounts for Boric Acid B(OH)3 relaxation, Magnesium Sulfate MgSO4 relaxation,
    and pure water viscous shear/bulk viscosity.
    """
    f = freq_khz
    T = temp_c
    S = salinity_ppt
    D = depth_m
    T_k = T + 273.15  # Kelvin

    # Sound speed for reference
    c = 1412.0 + 3.21 * T + 1.19 * S + 0.0167 * D

    # 1. Boric Acid Relaxation (0.1 - 2 kHz dominant, extended high freq)
    A1 = (8.86 / c) * (10.0 ** (0.78 * ph - 5.0))
    f1 = 2.8 * math.sqrt(S / 35.0) * (10.0 ** (4.0 - 1245.0 / T_k))
    P1 = 1.0  # Negligible pressure correction for shallow boric acid
    alpha_boric = (A1 * P1 * f1 * (f ** 2)) / ((f1 ** 2) + (f ** 2))

    # 2. Magnesium Sulfate (MgSO4) Relaxation (10 - 100+ kHz dominant)
    A2 = 21.44 * (S / c) * (1.0 + 0.025 * T)
    f2 = (8.17 * (10.0 ** (8.0 - 1990.0 / T_k))) / (1.0 + 0.0018 * (S - 35.0))
    P2 = 1.0 - 1.37e-4 * D + 6.2e-9 * (D ** 2)
    alpha_mgso4 = (A2 * P2 * f2 * (f ** 2)) / ((f2 ** 2) + (f ** 2))

    # 3. Pure Water Viscosity (Dominant above 100 kHz)
    if T <= 20.0:
        A3 = 4.937e-4 - 2.59e-5 * T + 9.11e-7 * (T ** 2) - 1.50e-8 * (T ** 3)
    else:
        A3 = 3.964e-4 - 1.146e-5 * T + 1.45e-7 * (T ** 2) - 6.5e-10 * (T ** 3)
    P3 = 1.0 - 3.83e-5 * D + 4.9e-10 * (D ** 2)
    alpha_h2o = A3 * P3 * (f ** 2)

    return alpha_boric + alpha_mgso4 + alpha_h2o

def sediment_scattering_attenuation(freq_khz: float, turbidity_ntu: float) -> float:
    """
    Thorne & Hanes (2002) sediment attenuation model (dB/km).
    Converts Turbidity (NTU) to sediment concentration M (g/L approx).
    alpha_sed = k_visc * M * f^0.5 + k_scat * M * a_p^3 * f^4
    """
    # Approximate relation: 100 NTU ~ 0.15 g/L suspended silt (mean radius 25 um)
    M = turbidity_ntu * 0.0015  # kg/m^3
    f_hz = freq_khz * 1e3

    # Viscous absorption in boundary layer
    alpha_visc = 0.024 * M * math.sqrt(freq_khz)  # dB/km approximation
    # Rayleigh scattering (dominant at high frequency >= 200 kHz)
    alpha_scat = 1.2e-5 * M * (freq_khz ** 2.2)

    return alpha_visc + alpha_scat

def compute_snr_margin(source_level_db: float, range_m: float, freq_khz: float,
                      temp_c: float, sal_ppt: float, depth_m: float,
                      turbidity_ntu: float, target_strength_db: float = -15.0,
                      noise_level_db: float = 55.0, di_db: float = 12.0) -> float:
    """
    Urick Active Sonar Equation:
    SNR = SL - 2*TL + TS - (NL - DI)
    where TL = 20*log10(R) + (alpha_seawater + alpha_sed) * R / 1000
    """
    alpha_sea = francois_garrison_absorption(freq_khz, temp_c, sal_ppt, depth_m)
    alpha_sed = sediment_scattering_attenuation(freq_khz, turbidity_ntu)
    total_alpha = alpha_sea + alpha_sed  # dB/km

    # Transmission loss (Spherical spreading + absorption)
    if range_m < 1.0:
        range_m = 1.0
    tl = 20.0 * math.log10(range_m) + (total_alpha * (range_m / 1000.0))
    snr = source_level_db - (2.0 * tl) + target_strength_db - (noise_level_db - di_db)
    return snr

def run_simulation_scenarios():
    print("=" * 80)
    print("  AERO-SONAR ACOUSTIC PROPAGATION SIMULATOR (SIH 2026 - PS 26058)")
    print("  Team AEROS (Team ID: 186870) | STM32G474RE Real-Time Edge Engine")
    print("=" * 80)
    print()

    scenarios = [
        {"name": "Clear Deep Pelagic", "depth": 250.0, "temp": 8.0, "sal": 35.0, "turb": 5.0, "target_dist": 600.0},
        {"name": "Warm Coastal Shallow", "depth": 15.0, "temp": 28.0, "sal": 33.0, "turb": 45.0, "target_dist": 150.0},
        {"name": "Turbid Estuarine Silt", "depth": 8.0, "temp": 22.0, "sal": 28.0, "turb": 280.0, "target_dist": 80.0},
        {"name": "High-Res Bathymetry", "depth": 40.0, "temp": 18.0, "sal": 34.5, "turb": 15.0, "target_dist": 40.0},
    ]

    freq_candidates = [70.0, 105.0, 140.0, 300.0, 500.0]

    for sc in scenarios:
        c = mackenzie_sound_speed(sc["temp"], sc["sal"], sc["depth"])
        print(f"--- Scenario: {sc['name']} ---")
        print(f"  Conditions: Depth={sc['depth']}m, Temp={sc['temp']}C, Salinity={sc['sal']}ppt, Turbidity={sc['turb']}NTU")
        print(f"  Calculated Mackenzie Sound Speed: {c:.2f} m/s")
        print(f"  Target Range: {sc['target_dist']} m")
        print("  Frequency Evaluation:")
        print(f"  {'Freq (kHz)':<12} {'Alpha (dB/km)':<16} {'2*TL (dB)':<14} {'Echo SNR (dB)':<14} {'Feasibility'}")
        print("  " + "-" * 70)

        for f in freq_candidates:
            alpha_tot = francois_garrison_absorption(f, sc["temp"], sc["sal"], sc["depth"]) + \
                        sediment_scattering_attenuation(f, sc["turb"])
            tl = 20.0 * math.log10(sc["target_dist"]) + (alpha_tot * (sc["target_dist"] / 1000.0))
            two_tl = 2.0 * tl
            # Assume 195 dB source level
            snr = compute_snr_margin(195.0, sc["target_dist"], f, sc["temp"], sc["sal"], sc["depth"], sc["turb"])
            feasibility = "OPTIMAL" if snr >= 15.0 else ("MARGINAL" if snr >= 6.0 else "UNVIABLE")
            print(f"  {f:<12.1f} {alpha_tot:<16.2f} {two_tl:<14.2f} {snr:<14.2f} {feasibility}")
        print()

if __name__ == "__main__":
    run_simulation_scenarios()

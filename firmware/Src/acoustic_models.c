/**
  ******************************************************************************
  * @file           : acoustic_models.c
  * @brief          : Implementation of physical ocean acoustic algorithms
  * @team           : Team AEROS (Team ID: 186870) - SIH 2026 PS 26058
  ******************************************************************************
  */

#include "acoustic_models.h"
#include <math.h>

/* Tabulated Francois-Garrison (1982) Reference Grid across 50 to 500 kHz */
/* Base alpha in dB/km at standard ocean baseline (10°C, 35 ppt, 50m depth) */
static const float FG_GRID_FREQ[10]  = {50.0f, 100.0f, 150.0f, 200.0f, 250.0f, 300.0f, 350.0f, 400.0f, 450.0f, 500.0f};
static const float FG_GRID_ALPHA[10] = {12.8f,  34.2f,  48.5f,  61.0f,  74.5f,  88.0f, 102.0f, 116.0f, 129.5f, 143.0f};

float Acoustic_ComputeSoundSpeed(float temp_c, float salinity_ppt, float depth_m)
{
    /* Mackenzie (1981) nine-term empirical sound speed formula */
    float T = temp_c;
    float S = salinity_ppt;
    float D = depth_m;

    float c = 1448.96f + 4.591f * T - 5.304e-2f * T * T + 2.374e-4f * T * T * T +
              1.340f * (S - 35.0f) + 1.63e-2f * D + 1.675e-7f * D * D -
              1.025e-2f * T * (S - 35.0f) - 7.139e-13f * T * D * D * D;

    return c;
}

float Acoustic_ComputeFrancoisGarrison(float f_khz, float temp_c, float salinity_ppt, float depth_m)
{
    float base_alpha = 88.0f;
    float f = f_khz;

    if (f < 50.0f)  f = 50.0f;
    if (f > 500.0f) f = 500.0f;

    /* Piecewise linear interpolation across tabulated Francois-Garrison grid */
    if (f <= FG_GRID_FREQ[0]) {
        base_alpha = FG_GRID_ALPHA[0];
    } else if (f >= FG_GRID_FREQ[9]) {
        base_alpha = FG_GRID_ALPHA[9];
    } else {
        for (int i = 0; i < 9; i++) {
            if (f >= FG_GRID_FREQ[i] && f <= FG_GRID_FREQ[i + 1]) {
                float ratio = (f - FG_GRID_FREQ[i]) / (FG_GRID_FREQ[i + 1] - FG_GRID_FREQ[i]);
                base_alpha = FG_GRID_ALPHA[i] + ratio * (FG_GRID_ALPHA[i + 1] - FG_GRID_ALPHA[i]);
                break;
            }
        }
    }

    /* Environmental physical scaling factors: */
    /* Salinity: MgSO4 relaxation scales with S/35 (Bay of Bengal freshwater dilution effect) */
    float salinity_factor = 0.15f + 0.85f * (salinity_ppt / 35.0f);

    /* Temperature: alpha decreases ~0.7% per °C above 10°C */
    float temp_factor = 1.0f - 0.007f * (temp_c - 10.0f);

    /* Hydrostatic pressure / depth attenuation correction */
    float depth_factor = 1.0f - 0.0004f * (depth_m / 10.0f);

    return base_alpha * salinity_factor * temp_factor * depth_factor;
}

float Acoustic_ComputeTurbidityScattering(float turbidity_ntu, float f_khz)
{
    if (turbidity_ntu <= 0.0f) return 0.0f;
    /* Thorne & Hanes (2002) sediment particulate scattering: ~0.18 dB/km per NTU @ 100 kHz */
    return 0.180f * turbidity_ntu * powf(f_khz / 100.0f, 1.25f);
}

float Acoustic_ComputeDopplerIndex(float speed_ms, float bandwidth_hz, float duration_sec, float c_ms)
{
    if (c_ms <= 100.0f) c_ms = 1500.0f;
    return (2.0f * speed_ms * bandwidth_hz * duration_sec) / c_ms;
}

float Acoustic_ComputeUsableRange(float sl_db, float nl_db, float alpha_total_db_km)
{
    float di = 14.0f;
    float ts = -14.0f;
    float dt = 8.0f;
    float safety_margin = 22.0f;
    float max_2tl = sl_db - (nl_db - di) + ts - dt - safety_margin;

    float r = 12.0f;
    for (float test_r = 12.0f; test_r <= 220.0f; test_r += 1.0f) {
        float two_way_tl = 40.0f * log10f(test_r) + 2.0f * (alpha_total_db_km / 1000.0f) * test_r;
        if (two_way_tl > max_2tl) {
            r = test_r;
            break;
        }
        r = test_r;
    }
    return r;
}

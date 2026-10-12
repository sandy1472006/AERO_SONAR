/**
  ******************************************************************************
  * @file           : ewa_decision_engine.c
  * @brief          : EWA decision engine implementation with deterministic safety clamps
  * @team           : Team AEROS (Team ID: 186870) - SIH 2026 PS 26058
  ******************************************************************************
  */

#include "ewa_decision_engine.h"
#include "acoustic_models.h"
#include <math.h>

void EWA_Init(void)
{
    /* Initialize internal static lookup tables and state filters */
}

EnvironmentState_t EWA_SanitizeInputs(const float raw_adcs[NUM_EMULATED_SENSORS])
{
    EnvironmentState_t env;

    /* Normalize 0.0V - 3.3V ADC values into physical marine sensor units */
    /* POT1: Turbidity (0 - 100 NTU) */
    env.turbidity_ntu = (raw_adcs[0] / 3.3f) * 100.0f;
    if (env.turbidity_ntu < 0.0f)   env.turbidity_ntu = 0.0f;
    if (env.turbidity_ntu > 100.0f) env.turbidity_ntu = 100.0f;

    /* POT2: Water Depth (0 - 200 m) */
    env.depth_m = (raw_adcs[1] / 3.3f) * 200.0f;
    if (env.depth_m < 0.0f)   env.depth_m = 0.0f;
    if (env.depth_m > 200.0f) env.depth_m = 200.0f;

    /* POT3: Temperature (0 - 30 °C) */
    env.temp_c = (raw_adcs[2] / 3.3f) * 30.0f;
    if (env.temp_c < 0.0f)  env.temp_c = 0.0f;
    if (env.temp_c > 30.0f) env.temp_c = 30.0f;

    /* POT4: Salinity (25 - 38 ppt) */
    env.salinity_ppt = 25.0f + (raw_adcs[3] / 3.3f) * (38.0f - 25.0f);
    if (env.salinity_ppt < 25.0f) env.salinity_ppt = 25.0f;
    if (env.salinity_ppt > 38.0f) env.salinity_ppt = 38.0f;

    /* POT5: Battery State (0 - 100 %) */
    env.battery_pct = (raw_adcs[4] / 3.3f) * 100.0f;
    if (env.battery_pct < 0.0f)   env.battery_pct = 0.0f;
    if (env.battery_pct > 100.0f) env.battery_pct = 100.0f;

    /* POT6: Target Range (10 - 200 m) */
    env.target_range_m = 10.0f + (raw_adcs[5] / 3.3f) * (200.0f - 10.0f);
    if (env.target_range_m < 10.0f)  env.target_range_m = 10.0f;
    if (env.target_range_m > 200.0f) env.target_range_m = 200.0f;

    /* POT7: Vehicle Velocity (0.0 - 3.0 m/s) */
    env.speed_ms = (raw_adcs[6] / 3.3f) * 3.0f;
    if (env.speed_ms < 0.0f) env.speed_ms = 0.0f;
    if (env.speed_ms > 3.0f) env.speed_ms = 3.0f;

    /* POT8: Ambient Acoustic Noise (30 - 80 dB) */
    env.noise_db = 30.0f + (raw_adcs[7] / 3.3f) * (80.0f - 30.0f);
    if (env.noise_db < 30.0f) env.noise_db = 30.0f;
    if (env.noise_db > 80.0f) env.noise_db = 80.0f;

    return env;
}

WaveformDecision_t EWA_ComputeDecision(const EnvironmentState_t *env)
{
    WaveformDecision_t d;
    d.clamped_amplitude = false;
    d.clamped_duration  = false;
    d.clamped_frequency = false;
    d.waveform          = WAVEFORM_LFM_CHIRP;

    /* Safety & Emergency Corridor Triggers */
    bool is_power_save = (env->battery_pct <= 20.0f);
    bool is_impossible_link = (env->target_range_m > 175.0f && env->turbidity_ntu > 75.0f && env->noise_db > 68.0f);

    if (is_impossible_link) {
        /* SAFE-FALLBACK: Severe link margin failure -> lock to robust 100 kHz baseline */
        d.corridor          = CORRIDOR_SAFE_FALLBACK;
        d.f_center_hz       = 100000.0f;
        d.duration_ms       = 2.00f;
        d.amplitude_pct     = 95.0f;
        d.clamped_amplitude = true;
        d.clamped_duration  = true;
        d.clamped_frequency = true;
    }
    else if (is_power_save) {
        /* POWER-SAVE: Conserve battery reserves -> clamp amplitude to 35% ceiling */
        d.corridor          = CORRIDOR_POWER_SAVE;
        d.f_center_hz       = 150000.0f;
        d.duration_ms       = 0.60f;
        float scaled_amp    = env->target_range_m * 0.25f;
        if (scaled_amp < 20.0f) scaled_amp = 20.0f;
        if (scaled_amp > 35.0f) scaled_amp = 35.0f;
        d.amplitude_pct     = scaled_amp;
        d.clamped_amplitude = true;
        d.clamped_duration  = true;
    }
    else {
        /* Continuous Multi-Objective Acoustic Optimization (100 kHz - 500 kHz) */
        /* High authority allocated to PS Core Sensors: Depth (35%) & Turbidity (40%) */
        float depth_factor = env->depth_m / 200.0f;
        float turb_factor  = env->turbidity_ntu / 100.0f;
        float range_factor = env->target_range_m / 200.0f;

        float penetration_demand = depth_factor * 0.35f + turb_factor * 0.40f + range_factor * 0.25f;
        if (penetration_demand > 1.0f) penetration_demand = 1.0f;

        /* Continuous carrier frequency mapping between 500 kHz (shallow/clear) and 100 kHz (deep/turbid) */
        float f_target = 500000.0f - penetration_demand * 400000.0f;
        /* Quantize to discrete 2.5 kHz grid for stable LC network resonance */
        d.f_center_hz = roundf(f_target / 2500.0f) * 2500.0f;

        if (d.f_center_hz <= 180000.0f) {
            d.corridor = CORRIDOR_LONG_RANGE;
        } else if (d.f_center_hz >= 380000.0f) {
            d.corridor = CORRIDOR_HIGH_RES;
        } else {
            d.corridor = CORRIDOR_BALANCED;
        }

        /* Pulse duration scales with range to maintain SNR processing gain */
        d.duration_ms = 0.60f + (env->target_range_m / 200.0f) * 1.20f;
        d.duration_ms = roundf(d.duration_ms * 20.0f) / 20.0f; /* 0.05ms steps */

        /* Active Sonar Equation Amplitude Optimization */
        float noise_factor = powf(10.0f, (env->noise_db - 50.0f) / 40.0f);
        float range_loss   = powf(env->target_range_m / 100.0f, 1.20f);
        float calc_amp     = 40.0f + 35.0f * range_loss * noise_factor;
        if (calc_amp > 100.0f) {
            calc_amp = 100.0f;
            d.clamped_amplitude = true;
        }
        if (calc_amp < 30.0f) calc_amp = 30.0f;
        d.amplitude_pct = calc_amp;
    }

    /* Fractional Bandwidth: 15% of center frequency (Piezoelectric resonance matching) */
    d.bandwidth_hz = roundf(d.f_center_hz * 0.15f);
    d.f_start_hz   = roundf(d.f_center_hz - d.bandwidth_hz / 2.0f);
    d.f_stop_hz    = roundf(d.f_center_hz + d.bandwidth_hz / 2.0f);

    /* Hardware DAC Clamp Boundaries (85 kHz - 545 kHz) */
    if (d.f_start_hz < 85000.0f) {
        d.f_start_hz = 85000.0f;
        d.clamped_frequency = true;
    }
    if (d.f_stop_hz > 545000.0f) {
        d.f_stop_hz = 545000.0f;
        d.clamped_frequency = true;
    }

    return d;
}

AcousticDerived_t EWA_ComputeDerived(const EnvironmentState_t *env, const WaveformDecision_t *decision)
{
    AcousticDerived_t dev;

    dev.sound_speed_ms = Acoustic_ComputeSoundSpeed(env->temp_c, env->salinity_ppt, env->depth_m);
    float f_khz = decision->f_center_hz / 1000.0f;
    float alpha_sw   = Acoustic_ComputeFrancoisGarrison(f_khz, env->temp_c, env->salinity_ppt, env->depth_m);
    float alpha_turb = Acoustic_ComputeTurbidityScattering(env->turbidity_ntu, f_khz);
    dev.alpha_total_db_km = alpha_sw + alpha_turb;

    /* Slant Range Resolution Delta_R = c / (2 * B) */
    dev.resolution_cm = (dev.sound_speed_ms / (2.0f * decision->bandwidth_hz)) * 100.0f;

    /* Doppler Index D */
    float tau_sec = decision->duration_ms / 1000.0f;
    dev.doppler_d = Acoustic_ComputeDopplerIndex(env->speed_ms, decision->bandwidth_hz, tau_sec, dev.sound_speed_ms);

    /* Usable acoustic range calculation via Active Sonar Equation */
    float sl = 174.0f + 20.0f * log10f(decision->amplitude_pct / 100.0f);
    dev.usable_range_m = Acoustic_ComputeUsableRange(sl, env->noise_db, dev.alpha_total_db_km);

    /* Energy per ping integration: E = A^2 * tau * power_coeff */
    float a_norm = decision->amplitude_pct / 100.0f;
    dev.energy_per_ping_mj = (a_norm * a_norm) * decision->duration_ms * 80.0f;

    /* Seabed propagation check */
    float est_altitude = (env->depth_m * 0.45f > 12.0f) ? env->depth_m * 0.45f : 12.0f;
    dev.reaches_seabed = (dev.usable_range_m >= est_altitude * 1.05f);

    return dev;
}

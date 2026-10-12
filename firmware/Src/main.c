/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main executive loop for AERO-SONAR Adaptive Transmitter
  *                   Target: STM32G474RET6 @ 170 MHz
  * @team           : Team AEROS (Team ID: 186870) - SIH 2026 PS 26058
  ******************************************************************************
  */

#include "main.h"
#include "ewa_decision_engine.h"
#include "cordic_waveform_synth.h"
#include "acoustic_models.h"
#include <stdio.h>
#include <string.h>

/* Global State */
static EnvironmentState_t   g_env;
static WaveformDecision_t   g_decision;
static AcousticDerived_t    g_derived;

/* Raw ADC voltages for 8 emulated sensors */
static float g_raw_adcs[NUM_EMULATED_SENSORS] = {
    0.396f, /* POT1: Turbidity ~12 NTU */
    0.412f, /* POT2: Depth ~25 m */
    2.420f, /* POT3: Temperature ~22 °C */
    2.538f, /* POT4: Salinity ~35 ppt */
    2.805f, /* POT5: Battery ~85 % */
    0.907f, /* POT6: Range ~55 m */
    1.320f, /* POT7: Speed ~1.2 m/s */
    1.056f  /* POT8: Noise ~46 dB */
};

/* Telemetry Buffer */
static char g_telemetry_json[512];

/* Low-level peripheral stubs */
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_DAC1_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_CORDIC_Init(void);

static void Transmit_JSON_Telemetry(void);

int main(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    // HAL_Init();
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_ADC1_Init();
    MX_DAC1_Init();
    MX_TIM1_Init();
    MX_USART2_UART_Init();
    MX_CORDIC_Init();

    /* Initialize EWA Decision Engine & Waveform Synthesizer */
    EWA_Init();
    Waveform_Init();

    /* Initial state calculation */
    g_env = EWA_SanitizeInputs(g_raw_adcs);
    g_decision = EWA_ComputeDecision(&g_env);
    g_derived = EWA_ComputeDerived(&g_env, &g_decision);
    Waveform_Configure(&g_decision);

    uint32_t last_sensor_tick = 0;
    uint32_t last_telemetry_tick = 0;
    uint32_t current_tick = 0;

    /* Infinite Super-Loop (20 Hz Adaptive Rate) */
    while (1)
    {
        current_tick++;

        /* 1. Sensor Acquisition & Adaptation Loop (Runs every 50ms = 20 Hz) */
        if (current_tick - last_sensor_tick >= 50)
        {
            last_sensor_tick = current_tick;

            /* Ingest ADC sensor voltages */
            g_env = EWA_SanitizeInputs(g_raw_adcs);

            /* Run pure EWA Decision Engine */
            g_decision = EWA_ComputeDecision(&g_env);

            /* Compute physical link derivations & link budget */
            g_derived = EWA_ComputeDerived(&g_env, &g_decision);

            /* Configure CORDIC waveform generator for next ping */
            Waveform_Configure(&g_decision);
        }

        /* 2. WebSerial JSON Telemetry Output (Runs at 10 Hz) */
        if (current_tick - last_telemetry_tick >= 100)
        {
            last_telemetry_tick = current_tick;
            Transmit_JSON_Telemetry();
        }
    }
}

static void Transmit_JSON_Telemetry(void)
{
    const char *corridor_str = "BALANCED";
    if (g_decision.corridor == CORRIDOR_HIGH_RES)      corridor_str = "HIGH-RES";
    else if (g_decision.corridor == CORRIDOR_LONG_RANGE)   corridor_str = "LONG-RANGE";
    else if (g_decision.corridor == CORRIDOR_POWER_SAVE)   corridor_str = "POWER-SAVE";
    else if (g_decision.corridor == CORRIDOR_SAFE_FALLBACK) corridor_str = "SAFE-FALLBACK";

    snprintf(g_telemetry_json, sizeof(g_telemetry_json),
        "{\"depth\":%.1f,\"temp\":%.1f,\"sal\":%.1f,\"turb\":%.1f,\"range\":%.1f,\"speed\":%.1f,\"batt\":%.0f,\"noise\":%.1f,"
        "\"corridor\":\"%s\",\"f_start\":%.0f,\"f_stop\":%.0f,\"bw\":%.0f,\"dur\":%.2f,\"amp\":%.0f,"
        "\"res_cm\":%.1f,\"range_m\":%.0f,\"energy_mj\":%.1f,\"c\":%.0f,\"alpha\":%.1f}\r\n",
        g_env.depth_m, g_env.temp_c, g_env.salinity_ppt, g_env.turbidity_ntu,
        g_env.target_range_m, g_env.speed_ms, g_env.battery_pct, g_env.noise_db,
        corridor_str, g_decision.f_start_hz, g_decision.f_stop_hz, g_decision.bandwidth_hz,
        g_decision.duration_ms, g_decision.amplitude_pct,
        g_derived.resolution_cm, g_derived.usable_range_m, g_derived.energy_per_ping_mj,
        g_derived.sound_speed_ms, g_derived.alpha_total_db_km
    );

    /* Output string over UART2 (HAL_UART_Transmit_DMA or polling) */
}

void SystemClock_Config(void)
{
    /* Configure 170 MHz System PLL from 8 MHz HSE crystal */
}

static void MX_GPIO_Init(void) {}
static void MX_DMA_Init(void) {}
static void MX_ADC1_Init(void) {}
static void MX_DAC1_Init(void) {}
static void MX_TIM1_Init(void) {}
static void MX_USART2_UART_Init(void) {}
static void MX_CORDIC_Init(void) {}

void Error_Handler(void)
{
    while (1) {}
}

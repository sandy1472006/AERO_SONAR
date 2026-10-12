/**
  ******************************************************************************
  * @file           : acoustic_models.h
  * @brief          : Ocean acoustic propagation models (Mackenzie, Francois-Garrison, Thorne-Hanes)
  * @team           : Team AEROS (Team ID: 186870) - SIH 2026 PS 26058
  ******************************************************************************
  */

#ifndef __ACOUSTIC_MODELS_H
#define __ACOUSTIC_MODELS_H

#include "main.h"

/* Compute sound speed in seawater using Mackenzie (1981) 9-term polynomial */
float Acoustic_ComputeSoundSpeed(float temp_c, float salinity_ppt, float depth_m);

/* Compute seawater absorption using Francois-Garrison (1982) relaxation grid */
float Acoustic_ComputeFrancoisGarrison(float f_khz, float temp_c, float salinity_ppt, float depth_m);

/* Compute suspended sediment acoustic scattering loss using Thorne & Hanes (2002) */
float Acoustic_ComputeTurbidityScattering(float turbidity_ntu, float f_khz);

/* Evaluate dimensionless Doppler Index D = (2 * v * B * tau) / c */
float Acoustic_ComputeDopplerIndex(float speed_ms, float bandwidth_hz, float duration_sec, float c_ms);

/* Compute active sonar equation usable range */
float Acoustic_ComputeUsableRange(float sl_db, float nl_db, float alpha_total_db_km);

#endif /* __ACOUSTIC_MODELS_H */

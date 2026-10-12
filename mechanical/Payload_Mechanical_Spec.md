# AERO-SONAR — AUV Payload Mechanical Integration Specification

**Smart India Hackathon 2026 — Problem Statement 26058**  
**Ministry of Earth Sciences (MoES) / National Institute of Ocean Technology (NIOT)**  
**Team AEROS (Team ID: 186870)**  

---

## 1. Mechanical Integration Objective

The AERO-SONAR transmitter is engineered as a **modular, self-contained payload pod** designed to integrate directly into the payload bay of autonomous underwater vehicles, with specific physical dimensions and buoyancy matched to India's indigenous **Maya AUV** developed by CSIR-National Institute of Oceanography (NIO), Goa.

### Reference Vehicle Benchmark: Maya AUV
- **Total Vehicle Length:** $\sim 1.7\,\text{m}$
- **Vehicle Hull Diameter:** $\sim 234\,\text{mm}$
- **Dry Mass:** $\sim 54\,\text{kg}$
- **Max Operating Depth:** $200\,\text{m}$ (with $2.01\,\text{MPa}$ hydrostatic pressure)
- **Endurance:** $7.2\,\text{hours}$ (at nominal $1.5\,\text{m/s}$ survey speed)

---

## 2. Payload Dimensions & Physical Properties

```text
┌────────────────────────────────────────────────────────────────────────┐
│                        PAYLOAD MECHANICAL SPECIFICATIONS               │
├────────────────────────────────────────────────────────────────────────┤
│                                                                        │
│  • Enclosure Form Factor: Hydrodynamic cylindrical body of revolution   │
│  • Total Axial Length: 200 mm                                          │
│  • External Diameter: 90 mm                                            │
│  • Internal Usable Bore Diameter: 78 mm                                │
│  • Wall Thickness: 6.0 mm (PVC-U prototype) / 4.5 mm (6061-T6 Al)      │
│  • Acoustic Transmit Aperture: Ø 40 mm forward transducer face         │
│  • Total Displaced Volume (V_disp): 1.272 Liters                       │
│  • Displaced Seawater Mass (Buoyancy): ~1.30 kg                        │
│  • Total Dry Mass (Hull + Transducer + Electronics): ~1.05 kg          │
│  • Net Buoyancy in Seawater: +0.25 kg (Slightly positive for recovery) │
│                                                                        │
└────────────────────────────────────────────────────────────────────────┘
```

---

## 3. Structural CAD Models & Files

The repository includes manufacturing-ready 3D CAD files located in [`mechanical/cad/`](cad/):

| CAD File | Format | Description |
|---|---|---|
| [`AUV_payload_streamlined.stl`](cad/AUV_payload_streamlined.stl) | STL | Main hydrodynamic cylindrical payload pod hull with conformal forebody and internal mounting grooves. |
| [`AUV_service_panel.stl`](cad/AUV_service_panel.stl) | STL | Quick-access conformal service hatch for battery / bench programming access without disassembling endcaps. |
| [`AUV_hinge_pin.stl`](cad/AUV_hinge_pin.stl) | STL | Precision 3.4 mm pivot dowel pin for service hatch articulation. |

---

## 4. Internal Electronics Chassis Architecture

Inside the cylindrical pressure boundary, electronics are mounted on a **sliding dual-rail chassis tray**:
1. **Lower Rail Tier:** Houses the Class-D power amplifier, LC impedance matching relays, and high-side current shunt. Thermally coupled to the aluminum endcap for conductive heat dissipation.
2. **Upper Rail Tier:** Houses the STM32G474RE Nucleo processing core, ADC filtering, and DAC reconstruction stages, isolated from high-current power switching.
3. **Fore Section:** Houses the potted Tonpilz piezoelectric projector, sealed behind a polyurethane acoustic matching window with acoustic impedance matched to seawater ($Z_0 \approx 1.54 \times 10^6\,\text{kg/m}^2\cdot\text{s}$).
4. **Aft Section:** Features double-sealed endcap with brass cable glands for power input, serial telemetry, and external hydrophone input.

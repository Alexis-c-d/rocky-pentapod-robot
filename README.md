# Rocky: Pentaradial Robot

A robot version of Rocky from *Project Hail Mary*. Side project, currently **paused** with one full limb built.

## Design
- Pentaradial body: 5 limbs, 4 DOF each (20 servos total)
- Shoulder with 3 DOF (yaw, pitch, roll) plus elbow
- Feetech STS3215 bus servos (1:191 gear ratio variant)
- ESP32 controller
- Each limb is about 45 cm from shoulder to foot
- Skeleton 3D printed; exterior shell designed separately

## Engineering notes
- The STS3215 is rectangular and its output shaft is off-center, which makes a compact shoulder hard. This was solved with a custom bracket designed for it.
- Planned magnetic feet: start with simple electromagnets (one per foot, driven by MOSFETs from the ESP32, timed to the stance phase) as proof of concept for walking on metal surfaces.

## Status
- [x] Servos, batteries, and 3D printer on hand
- [x] One complete limb assembled
- [ ] Remaining 4 limbs
- [ ] Servo ID assignment and bring-up for all 20 servos
- [ ] Body assembly
- [ ] Gait and choreography
- [ ] Electromagnet feet

## Tools
ESP32 · Feetech STS3215 · 3D printing · CAD

LA1K CW beacon project
======================

A 35-4400 MHz CW exciter design on a 75 x 45 mm 4-layer PCB. Will initially be used for a 70 MHz beacon in Norway (with a 25 W PA and an external 10 MHz OCXO), and eventually replace hardware in our existing 2m and 70cm beacons. A serial port is used for configuration of callsign, ramp time, frequency etc.

![First PCB soldered](https://github.com/la1k/la1k-cw-beacon/blob/master/img/IMAG1742.jpg?raw=true)

- **Microcontroller (Microchip PIC18F26K22):** Controls synthesizer, does CW timing, controls VGA gain.
- **Synthesizer (Analog Devices ADF4351):** Fractional-N synthesizer with VCO which generates the RF-signal. Only course control over output power.
- **VGA (Analog Devices ADL5330):** Amplifier with variable gain over a 60 dB span. Controlled by filtered PWM-signal from microcontroller and softens the keying transitions by controlling output power ramping. Also works as a buffer for an external PA.

# MI12 UTC Project

## Getting Started

### Prerequisites

#### Hardware

- A PC
- USB Cable x4
- ESP32 x4 Development Board

#### Software

##### ESP Libraries

- NimBLE Library
- Wifi

##### Frontend

- node.js
- npm (pnpm is preferred)

### ESP's Setup

Televerse into each ESP32 board and flash the corresponding code:

- ESP Mobile -> env:esp32_mobile
- ESP Beacon1 -> env:esp32_beacon__1__
- ESP Beacon2 -> env:esp32_beacon__2__
- ESP Beacon3 -> env:esp32_beacon__3__

### Front

```bash
cd front
(p)npm install
(p)npm run dev
```

## Ressources

- [Trilattération]()
- [NimBLE]() A low power Bluetooth library for embedded systems.
- [Kalman Filter]() A mathematical method for estimating the state of a system from noisy measurements.
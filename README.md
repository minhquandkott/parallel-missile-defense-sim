# Parallel Missile Interception Simulator

A parallel simulation of an anti-missile defense system inspired by Iron Dome. The battlefield is divided into radar zones, each monitored by an MPI process. Enemy missiles are launched from random positions and fall toward the ground. Each MPI process uses OpenMP to calculate whether it can intercept incoming missiles in its zone.

---

## Features

- Simulates a 2D battlefield with falling enemy missiles
- Radar zones distributed across MPI processes
- Each zone uses OpenMP to calculate interception in parallel
- Logs successful interceptions with missile ID and coordinates
- Designed for performance analysis and scaling experiments

---

## Build Instructions

Make sure you have **MPI** and **OpenMP** installed.  
Tested on Linux/macOS using `mpicc`.

```bash
git clone https://github.com/minhquandkott/parallel-missile-defense-sim.git
cd parallel-missile-defense-sim
make

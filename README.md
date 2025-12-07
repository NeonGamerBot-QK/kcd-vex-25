# kcd-vex-25

This is the source code for team 62885A, built with PROS and C++.

We also have a CAD designed by a team member which you can view [here](https://cad.onshape.com/documents/acd16ecb47494069b0b1e1c5/w/6365d58a066689ce2ca389c2/e/eeedad6d0cf568923f456097?renderMode=0&uiState=6930b0ce67f10096ac8c64da).

## Directory Structure

```
v0/
├── src/
│   ├── auton/
│   ├── intake/
│   ├── outtake/
│   ├── screen/
│   ├── globals.cpp
│   ├── main.cpp
│   └── odom.cpp
├── include/
│   ├── auton/
│   ├── intake/
│   ├── outtake/
│   ├── screen/
│   ├── globals.hpp
│   └── main.h
├── firmware/
└── static/
```

## Dependencies

- **PROS Kernel**: 4.2.1
- **LemLib**: 0.5.6 - Chassis control and odometry (which i have not finished)
- **robodash**: 2.3.1 - UI/Dashboard (inspo used it)
- **liblvgl**: 8.3.9 - Graphics library

## Build Commands

```bash
cd v0 && pros make

make -C v0 clean

pros upload
```

## License

See [LICENSE](LICENSE) for details.

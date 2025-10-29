# AGENTS.md - VEX Robotics PROS Project

## Build/Test Commands
- **Build**: `cd v0 and pros make`
- **Clean**: `make -C v0 clean`
- **Upload to robot**: `pros upload ` (requires PROS CLI)
- **Quick build**: `make -C v0 quick` (default target)

## Architecture
- **Platform**: VEX V5 robotics platform using PROS 4.2.1 kernel
- **Main code**: v0/src/main.cpp - entry point with initialize(), autonomous(), opcontrol()
- **Libraries**: LemLib (0.5.6) for chassis control, robodash (2.3.1) for UI, liblvgl (8.3.9) for graphics
- **Structure**: v0/src/ contains subsystems (intake/, screen/), v0/include/ for headers

## Code Style
- **Language**: C++ (C++17 compatible)
- **Includes**: Use "main.h" first, then library includes (lemlib/api.hpp, robodash/api.h)
- **Namespacing**: Use pros:: prefix (do NOT use `using namespace pros;`)
- **Naming**: camelCase for functions, snake_case for variables, PascalCase for classes
- **Motor ports**: Hardware-specific port numbers (e.g., pros::Motor intakeMotor(7))
- **Comments**: Doxygen-style for functions, minimal inline comments
- **Formatting**: Tabs for indentation, K&R brace style
- **IWYU pragma**: Add `// IWYU pragma: keep` for essential includes like lemlib

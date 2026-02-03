# Sim8086 - Modern 8086 Emulator & Assembler

![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)
![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)
![License](https://img.shields.io/badge/license-MIT-orange.svg)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux-lightgrey.svg)

**Sim8086** is a modular, cross-platform emulator for the Intel 8086 microprocessor, built with **C++17** and **Qt 6**.

Designed for students and enthusiasts, it provides a visual environment to write, compile, and debug 8086 Assembly code. Unlike traditional text-only emulators, Sim8086 features real-time register visualization, memory inspection, and virtual hardware peripherals.

---

## ✨ Key Features

* **⚡ Real-Time Emulation:** Step-by-step execution of 8086 instructions.
* **📝 Smart Code Editor:** Syntax highlighting, line numbers, and auto-completion for Assembly mnemonics.
* **📊 Visual Debugging:**
    * **Register View:** Watch `AX`, `BX`, `CX`, `DX`, `IP`, `SP`, and Flags change in real-time (changes highlighted in red).
    * **Memory View:** Inspect the raw RAM content at any segment:offset.
    * **Copy to Clipboard:** Right-click the register table to export state for analysis.
* **💻 Virtual DOS Terminal:** Full support for `INT 21h` interrupts (Service `09h` print string, `02h` print char).
* **🚦 Virtual Hardware:**
    * **Traffic Light Controller:** Control a visual traffic light on **Port 199**.
* **🛠 Integrated Tools:**
    * **Base Converter:** Convert between Hex, Dec, and Bin instantly.
    * **Calculator:** Helper for quick arithmetic checks.

---
"""

# --- PART 2: Installation & Build (Contains the Build Directory steps) ---
part2 = r"""
## 🚀 Installation & Build

### Prerequisites
* **C++ Compiler** (GCC, Clang, or MSVC) supporting C++17.
* **CMake** (Version 3.16 or higher).
* **Qt 6** (Core, Gui, Widgets modules).

### Building from Source

**1. Clone the Repository**
```bash
git clone [https://github.com/ekjotsinghmakhija/sim8086.git](https://github.com/ekjotsinghmakhija/sim8086.git)
cd sim8086
```

**2. Create Build Directory**
```bash
mkdir build
cd build
```

**3. Compile**
* **Linux / macOS:**
    ```bash
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make
    ```
* **Windows (MinGW / MSVC):**
    ```powershell
    cmake -B . -G "MinGW Makefiles" ..
    mingw32-make
    ```

**4. Run**
```bash
./Sim8086
```

## 📖 Usage Guide

### 1. Writing Code
Type your Assembly code in the left editor. The emulator supports standard Intel syntax.

**Supported Instructions:**
`MOV`, `ADD`, `SUB`, `MUL`, `DIV`, `INC`, `DEC`, `PUSH`, `POP`, `CALL`, `RET`, `JMP`, `CMP`, `JE`, `JNE`, `INT`, `OUT`, `HLT`.

**Directives:**
`ORG`, `DB`, `OFFSET`.

### 2. Virtual Hardware (Traffic Light)
Sim8086 includes a simulated traffic light connected to **I/O Port 199**.
* **Bit 0:** Green Light
* **Bit 1:** Yellow Light
* **Bit 2:** Red Light

**Example Code:**
```assembly
ORG 100h

start:
    MOV AL, 4      ; Binary 100
    OUT 199, AL    ; Turn on RED light

    MOV AL, 2      ; Binary 010
    OUT 199, AL    ; Turn on YELLOW light

    MOV AL, 1      ; Binary 001
    OUT 199, AL    ; Turn on GREEN light

    HLT            ; Stop execution
```

### 3. Debugging
* **Compile:** Checks syntax and prepares memory.
* **Step:** Execute one instruction at a time.
* **Run:** Execute automatically (approx 60 Hz).
* **Step Back:** Rewind execution to undo the last instruction (Time Travel Debugging).

---

## 📂 Project Structure

* `src/cpu8086.cpp` - The core emulation engine (Fetch-Decode-Execute).
* `src/managers/` - Handles orchestration between UI and CPU.
    * `execution_manager.cpp` - Manages the run loop and stepping logic.
    * `hardware_controller.cpp` - Manages virtual devices (Terminal, Lights).
* `src/components/` - Custom UI widgets.
    * `code_editor.cpp` - The text editor with syntax highlighting.
    * `register_view.cpp` - The table showing CPU state.
    * `traffic_light.cpp` - The custom widget drawing the lights.

---

## 🤝 Contributing

Contributions are welcome!
1.  Fork the Project.
2.  Create your Feature Branch (`git checkout -b feature/AmazingFeature`).
3.  Commit your Changes (`git commit -m 'Add some AmazingFeature'`).
4.  Push to the Branch (`git push origin feature/AmazingFeature`).
5.  Open a Pull Request.

---

## 📄 License

Distributed under the MIT License. See `LICENSE` for more information.

---

## 👨‍💻 Author

**Ekjot Singh Makhija**
* GitHub: [@ekjotsinghmakhija](https://github.com/ekjotsinghmakhija)

---
*Built with ❤️ in C++ and Qt.*

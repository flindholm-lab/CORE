# CORE
Atari ST game Core
<img width="634" height="445" alt="image" src="https://github.com/user-attachments/assets/af2ee271-30eb-44c3-b900-515db3137ed9" />

> An arcade vector infiltration game crafted for the **Atari ST** family of computers, inspired by the high-voltage neon aesthetics arcade games.

Written by **Ninjabuffy**.

---

## 🕹️ Overview

In **CORE**, you navigate the outer security rings of the Master Control Program. Alternating concentric rings rotate around the core at high velocity. Your objective is to time your radial jumps through rotating ion yellow jump gates, penetrating each defensive perimeter until you reach and destabilize the central Core.

---

## ✨ Features

- **50 FPS VBL Synchronization**: Rock-solid 50 Hz frame rate using double buffering, zero-tearing screen flips, and direct Shifter address synchronization.
- **Hardware Acceleration**: Automatic runtime detection of the **Atari Mega ST / STE Blitter** for instant screen clearing, with an optimized unrolled 32-bit CPU burst fallback for standard 520ST/1040ST machines.
- **Parallax Starfield**: 3-layer cosmic background with 56 drifting stars across deep space, mid-orbit, and foreground twinkling layers.
- **Radiant Vector Beacon & Motion Trail**:
  - Multi-layered glowing vector diamond player avatar.
  - 12-step continuous fading orbital comet tail that follows the player's curved trajectory.
  - Floating ion exhaust motes that detach and drift into the void as you maneuver and drop.
- **Dynamic Neon Palette**: Real-time palette modulation simulating phosphor glow, pulsating ion amber jump gates, and MCP cyan vectors.
- **YM2149 Chiptune Audio & SFX**:
  - Multi-channel tracker engine generating a clean synth lead and bassline.
  - Dedicated sound effects engine with dynamic frequency envelopes:
    - **Drop**: Descending pitch swoosh when diving into inner rings.
    - **Landing**: Bright ascending ping upon a successful gate intercept.
    - **Crash**: Low impact thud on shield loss.
    - **Core Penetration**: Triumphant ascending arpeggio fanfare.
  - In-game music toggle (`M` key) at any time.
- **Persistent High Scores (`CORE.HI`)**:
  - Native bare-metal GEMDOS file I/O with magic header validation.
  - **Floppy & Hard Drive Compatible**: Automatic drive detection preserves YM2149 Port A floppy select signals so high scores save reliably to floppy disk (`A:\CORE.HI`) on real hardware or hard disk (`C:\CORE.HI`) in emulators.
- **Three Sector Difficulties**:
  - **Novice**: Wide gates, gentle rotation speeds.
  - **Pilot**: Standard arcade intercept timing.
  - **Master**: Narrow gates, rapid counter-rotations.

---

## 🎮 Controls

| Key | Action |
|:---:|:---|
| <kbd>Space</kbd> | **Drop / Dive** into the next inner ring (or start game / reboot) |
| <kbd>M</kbd> | **Toggle Chiptune Music** (On / Muted) |
| <kbd>1</kbd>, <kbd>2</kbd>, <kbd>3</kbd> | Select Difficulty (**Novice**, **Pilot**, **Master**) on Title Screen |
| <kbd>D</kbd> | Cycle Difficulty on Title Screen |
| <kbd>Esc</kbd> | Exit to desktop / TOS |

---

## 🎯 Gameplay Mechanics

1. **Orbit & Timing**: Your beacon orbits the active ring. Consecutive rings rotate in opposite directions.
2. **Jump Gates**: Each ring contains an **Ion Yellow** gap. When your beacon aligns with the future gate position of the next inner ring, your intercept locks.
3. **The Drop**: Press <kbd>Space</kbd> to initiate a radial vector drop. 
   - **Clean Intercept**: You land on the inner ring, score bonus points based on sector difficulty and current level, and release a shower of cyan sparks.
   - **Barrier Collision**: If you drop into an energy barrier, you lose a shield and trigger screen shake and red warning sparks.
4. **The Core**: Breaching the innermost ring (Ring 0) launches you into the core vortex, completing the level and triggering an expanding victory pulse before advancing to the next sector.

---

## 🛠️ Building from Source

The project is built with **[vbcc](http://www.compilers.de/vbcc.html)** targeting Atari TOS (68000 bare-metal, no external C libraries required).

### Prerequisites
- `vbcc` with `vlink` and `vasm` installed for the `m68k-atari` target.

### Compilation Command

```bash
vc +tos -O2 -o core2.prg core2.c
```

The resulting [`core2.prg`](core2.prg) binary can be copied directly to an Atari ST floppy disk or hard drive.

---

## 💻 Running in Emulators (Hatari)

To run the game in **[Hatari](https://hatari.tuxfamily.org/)**:

```bash
hatari --machine ste \
  --tos tos162se.img \
  -d /path/to/game/folder \
  --auto core2.prg
```

Or copy `core2.prg` into a `.st` disk image and boot directly from floppy drive A:.

---

## 💾 Hardware Compatibility

Tested and verified on:
- **Atari 520ST / 1040ST / Mega ST** (TOS 1.02 / 1.04)
- **Atari STE / Mega STE** (TOS 1.62 / 2.06)
- **Hatari Emulator** (ST & STE modes)

---

## 📜 Credits

- **Game Design & Code**: [Ninjabuffy](https://github.com/Ninjabuffy)
- **Inspiration**: Bally Midway's *TRON* (1982)
- **Platform**: Atari Corporation (Jack Tramiel era, 1985–1993)

---

*End of Line.*

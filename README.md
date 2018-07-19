mdcorrupt
=======

mdcorrupt is a specialized game corrupter focused on making corrupted game files less prone to crashing. In its current state it supports many game systems and many different file types with special protections for most systems. This project is a command line tool which is utilized in a GUI frontend called "VineCorrupt".

Homepage: http://www.maiddog.com/projects/corrupter/index.php

## Implemented Systems

* **NES**
  * PRG-ROM has basic opcode protection
  * Accepted formats: .nes
* **SNES**
  * Basic opcode protections
  * Accepted formats: .smc, .sfc
* **N64**
  * Very crude opcode protection which may not be effective
  * Accepted formats: .z64, .v64, .n64
* **GameCube**
  * Protections based on individual file types
  * Accepted formats: None. Use --nintendo flag on individual files.
* **Wii**
  * Protections based on individual file types
  * Accepted formats: None. Use --nintendo flag on individual files.
* **GameBoy and GameBoy Color**
  * Opcode protection
  * Accepted formats: .gb, gbc
* **GameBoy Advance**
  * Opcode protection
  * Accepted formats: .gba
* **NDS**
  * Protections based on individual file types
  * Accepted formats: .nds
* **Sega Genesis**
  * Vector table patching
  * Accepted formats: .bin, .md, .smd
* **PlayStation 1**
  * No protection
  * Accepted formats: .bin, .img
* **PlayStation Portable**
  * No protection
  * Accepted formats: .iso

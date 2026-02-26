# VIC-BBS
VIC-20 Bulletin Board System

![ds-bbs](https://repository-images.githubusercontent.com/888754159/6d61e2cc-c19a-4885-9846-0ec71defc770)

Back in 1983, I got my first computer - a VIC-20. I built a simple BBS program for it and ran it initially on my VIC-1600 VICModem. I had to answer each call manually! Quickly I got a 1650 modem that would auto-answer. I ran that BBS (called the Live Wire BBS) for many years before eventually moving on to running a Commodore 64 BBS (Theatre of Pain BBS) and then in the early 90's, a PC-based BBS (The Danger Zone BBS). I lost the original VIC-BBS diskettes and I've always wanted to re-do it.

Fast forward 40+ years, and I have that opportunity. WiFi modems, memory expanders and SD card readers are all now available for the VIC-20 (and C64/C128) which makes building and running a BBS much easier. This program is the result of several months of hard work, preceded by a few years of pondering and experimenting with code.

## Quick Start
- Download the latest release and transfer the .PRG files to your 35K expanded VIC-20.
- If using VICE emulation configure VICE's RS232 parameters, and ensure tcpser is running with the correct parameters.
  - VICE EXAMPLE:
  - Settings > Peripheral Devices > RS232
  - Enable Userport RS232 Emulation [checked]
  - RS-232 Interface Type [Normal Control Lines]
  - Device: Serial 3, Baud: 1200
  - Serial 3: 127.0.0.1:25232, Baud: 1200, IP232 [checked]
  - tcpser command line: tcpser -v 25232 -s 1200 -l 4 -p 6502 -i "e0s1s0=1s2=255s30=30&k0"
- If using a real VIC-20, transfer the .PRG files to a 1541 diskette and ensure set your WiFi modem to answer calls.
  - WIFI MODEM EXAMPLE:
  - set baud rate to 1200
  - set "e0s1s0=1s2=255s30=30&k0" as the default init string
- load + run the "BBS" program, which will auto create a user log for you on first run.
- This will create a new user log with just 1 record:
  - USER ID: 1
  - USERNAME: SYSOP
  - PASSWORD: SYSOP
- You'll need to create some sequential files to support the board's operation (see below).
- Load and run "BBS". Press F1 to enter Local Mode, or F7 to Exit the Call Waiting screen.

## The BBS Program
The program itself is in many parts (PRG files):
- BBS : This is a BASIC program that provides the vast majority of the functions of the BBS.
- BOOTSTRAP : This is a special ML program that allows the BBS to swap in/out modules as required.
- EDITOR : This is the message editor module, and also used by the SYSOP module for editing SEQ files.
- FILES : This is the Xmodem upload/download module.
- GAMES : This is the online games module.
- SYSOP : This is the sysop module that lets you manage files and users.

## Required SEQ Files
In order to run the BBS, you are going to need some supporting sequential (text) files. These can contain any PETSCII codes you would like. These are included on the D64 image in the repo.

You need to create the following SEQ files:
- BANNER : Splash screen showed to user upon initial connect
- MOTD : Message of the Day shown after user logon
- MENU1 : Help menu for users with Security Level 1
- MENU2 : Help menu for users with Security Level 2
- MENU3 : Help menu for users with Security Level 3
- MENU4 : Help menu for users with Security Level 4
- MENU5 : Help menu for users with Security Level 5
- MENU6 : Help menu for users with Security Level 6
- THE WALL H : Header file for The Wall
- THE WALL : Actual entries for The Wall (newest on top)
- FILELIST : File listing with descriptions
- UPNOTE : Message displayed before allow users to upload
- UPLOADS : Hidden file that gets uploaded file descriptions from users
- GAMELIST : List of online games
- NEWUSER1 : Shown to new users prior to filling out application
- NEWUSER2 : Shown to new users after filling out application
- LOGOFF : Shown to users at logoff time
- BANNED : Shown to users who have security level zero (banned)
- TIMEUP : Shown to users when their time has expired
- LOGFULL : Shown to a new user when the user log is full (255 users max)
- INFO : Display information regarding the BBS
- BULLETINS : Listing of available bulletins on the system (numbered 1 to 255)
- B 1 - first bulletin
- B 2 - second bulletin  
- SYSOPMENU : Help Menu for the Sysop area

## Real Time Clock
The VIC has no way of knowing what the current date is, and it's time keeping ability is hampered by the fact that the internal timer pauses whenver the disk is accessed. This makes using an external real time clock (RTC) chip required to run the BBS effectively. In common.c, there are two functions at called getdate() and gettime(). You will likely need to modify those two functions to work with whatever RTC solution you have chosen. In VICE, a DS12C887 is emulated and my physical VIC uses a VIC-2407 cartridge made by Jaystonian's Retro Shop on Tindie that includes a DS12885/87 RTC chip and 35KB of expansion RAM. So my code reflects how that chip works.

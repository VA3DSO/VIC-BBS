# VIC-BBS
VIC-20 Bulletin Board System

![ds-bbs](https://repository-images.githubusercontent.com/888754159/6d61e2cc-c19a-4885-9846-0ec71defc770)

Back in 1983, I got my first computer - a VIC-20. I built a simple BBS program for it and ran it initially on my VIC-1600 VICModem. I had to answer each call manually! Quickly I got a 1650 modem that would auto-answer. I ran that BBS (called the Live Wire BBS) for many years before eventually moving on to running a Commodore 64 BBS (Theatre of Pain BBS) and then in the early 90's, a PC-based BBS (The Danger Zone BBS). I lost the original VIC-BBS diskettes and I've always wanted to re-do it.

Fast forward 40+ years, and I have that opportunity. WiFi modems, memory expanders and SD card readers are all now available for the VIC-20 (and C64/C128) which makes building and running a BBS much easier. This program is the result of about 2 weeks of hard work, preceded by a few years of pondering and experimenting with code.

## VIC-20 Hardware Requirements
To run this BBS, you'll need a VIC-20 computer with the following additional hardware:
- StrikeLink / WiFi modem (or compatible) that supports the Hayes command set
- 35K RAM expander such as the VIC-2407 cartridge
- real time clock chip (ie: DS12885T+ RTC chip) such as the one included in the VIC-2407 cartridge
- CMD hard disk (or compatible) or SD card drive (ie: SD2IDE)
  
## Quick Start
- Download the D64 image (programmed to work with VICE - if you're running on real hardware, you'll need to edit the .C code and compile for your particular configuration - see below).
- If using emulation, load the D64 image in VICE, configure VICE's RS232 parameters, and ensure tcpser is running with the correct parameters.
  - VICE EXAMPLE:
  - Settings > Peripheral Devices > RS232
  - Enable Userport RS232 Emulation [checked]
  - RS-232 Interface Type [Normal Control Lines]
  - Device: Serial 3, Baud: 1200
  - Serial 3: 127.0.0.1:25232, Baud: 1200, IP232 [checked]
  - tcpser command line: tcpser -v 25232 -s 1200 -l 4 -p 6502 -i "e0q1s0=1s2=255s30=30&k0"
- If using a real VIC-20, transfer the .PRG files to a 1541 diskette and ensure set your WiFi modem to answer calls.
  - WIFI MODEM EXAMPLE:
  - set baud rate to 1200
  - set "e0s1s0=1s2=255s30=30&k0" as the default init string
- load + run the "BBS" program
- it should alert you that the USER file is missing and advises you to create it from the Sysop menu
- the board will then load up to the call waiting screen
- press the F1 key to go into local mode
- at the COMMAND: prompt, enter Z for the Sysop menu
- at the SYSOP: prompt, enter C to create a new User Log
- this will create a new user log with just 1 record:
  - USER ID: 1
  - USERNAME: SYSOP
  - PASSWORD: SYSOP
- You'll need to create some sequential files to support the board's operation (see below).

## The BBS Program
The program itself is in six parts (PRG files):
- BBS : This is the main module that supports answer calls, showing Bulletins, reading Messages, and some utiltiies like System Information and Change Your Password.
- EDITOR : This is the module for writing Messages and allows the Sysop to edit any SEQ file.
- FILES : This is the Xmodem Upload and Download module.
- GAMES : This is the online games module.
- SYSOP : A group of tools and utilities that allow the Sysop to manage the BBS.
- BOOTSTRAP : A small machine language program that allows the BBS to swap between modules.

## Memory Usage
The VIC-BBS program uses the following Zero Page addresses when moving between modules via the Bootstrap program:
- 251 | $FB : LOCALMODE (is the user logged in locally? TRUE/FALSE, 1/0)
- 252 | $FC : U.ID (user ID of logged in user, 1-254)
- 253 | $FD : MODULE (which module to boot into, 1-6)

The high block of 40960 to 49151 ($A000 - $BFFF) which is 8K in size is used by the Message Editor as well as the Bootstrap program.
  
## Required SEQ Files
In order to run the BBS, you are going to need some supporting sequential (text) files. These can contain any PETSCII codes you would like. These are not included with BBS.

You need to create the following SEQ files:
- BANNER : Splash screen showed to user upon initial connect
- MOTD : Message of the Day shown after user logon
- HELP : Main Menu help screen
- NEWUSER1 : Shown to new users prior to filling out application
- NEWUSER2 : Shown to new users after filling out application
- LOGOFF : Shown to users at logoff time
- BANNED : Shown to users who have security level zero (banned)
- TIMEUP : Shown to users when their time has expired
- LOGFULL : Shown to a new user when the user log is full (255 users max)
- MENU1 : Main menu listing for users with security level 1
- MENU2 : Main menu listing for users with security level 2
- MENU3 : Main menu listing for users with security level 3 (default user level)
- MENU4, MENU5, MENU6 : Main menu listings for users with security levels 4, 5, 6
- INFO : Display information regarding the BBS
- BULLETINS : Listing of available bulletins on the system (numbered 1 to 255)
- B 1 : first bulletin
- B 2 : second bulletin
- UPNOTE : a message displayed to users before uploading
- FILELIST : a manually curated list of files in the download section
- GAMELIST : a list of available online games
- THE WALL : a file to hold entries users post to "The Wall"
- SYSOPMENU : main menu for Sysop
- DOSSHELL : help menu for Sysop DOS shell
  
## Real VIC-20 vs VICE: Differences
The way the BBS answers calls is very simple. The "modem" is programmed to answer after one ring (ats0=1) and all the BBS does is monitor the user port (address 37136) for a carrier tone. On a physical VIC-20, that shows up on bit 3 (ie: AND 8 in BASIC or AND #$08 in ML) whereas in VICE it shows up on bit 4 (ie: AND 16 in BASIC or AND #$10 in ML). This is referenced in just one spot (common.c) in the carrierdetect() function. Update this depending on which platform you are using (the Release files are created with VICE in mind).
  
Note: Bits are numbered 0 to 7 (not 1 to 8) so bit 0=1, bit 1=2, bit 2=4, bit 3=8, bit 4=16, etc.  

## Real Time Clock
The VIC has no way of knowing what the current date is, and it's time keeping ability is hampered by the fact that the internal timer pauses whenver the disk is accessed. This makes using an external real time clock (RTC) chip required to run the BBS effectively. 

In common.c, there are two functions: getdate() and gettime() - in the release code, they are programmed to work with the DS12C887 that VICE emulates. You are going to have to adjust this code to work with your own RTC solution (ie: VIC-2407 cartridge). Ping me if you want to see my C code that works on a physical VIC using the VIC-2407 cartridge.

## Main Menu Functions
The following functions are available from the Main Menu. The key press that activates the menu item is shown in square brackets, and the security level required for each function is listed after it in round brackets:
- [B] Bulletins (1): Show the bulletins SEQ file and lets the user read the bulletin files (b 1, b 2, etc)
- [W] The Wall (2): View the entries in The Wall and post to it.
- [R] Read Messages (2): Allows the user to read through the message SEQ files (m 1, m 2, etc)
- [P] Post Message (3): Post a Message on the BBS (all messages are public)
- [G] Games (3): Play online games
- [C] Chat (1): Page the sysop for Chat
- [$] Directory (1): View the file names and block sizes in the Download directory
- [F] File Descriptions (2): Show the details of each file in the Download directory
- [D] Download (3): files using Xmodem
- [U] Upload (3): files using Xmodem
- [I] Info (1): Shows the info SEQ file
- [V] View Users (1): Show the User List for the BBS (user id and username)
- [S] Set Password (1): Change your password
- [T] Time Left (1): Show how many minutes remaining in the current call
- [O] Log Off (1): End the call
- [H] or [?] Help (1): Show the menu SEQ for the user's security level (ie: menu3)

## Sysop Menu Functions
The following functions are available to the Assistant Sysop (5) and Sysop (6):
- [A] Add User (5): Add a new user to the user log
- [E] Edit User (5): Edit an existing user (need to know their user ID)
- [G] Gen User List (5): Generate User List from User Log
- [F] File Editor (5): Edit any SEQ file on the system
- [V] View Users (5): View the User List (same as Main Menu function)
- [D] Delete User (5): Delete a User
- [B] Backup User Log (5): Backup the User Log REL file to SEQ file
- [R] Restore User Log (5): Restore the User Log REL file from SEQ file
- [S] Fix System Stats (5): Fix last caller, number of users, messages, bulletins, etc
- [C] Create User Log (6): Destroys current User Log and writes brand new EMPTY one [DANGER]
- [X] Exit Sysop Menu (5): Return to the Main Menu
- [H] or [?] Help (5): Display the Sysop menu

## About Xmodem File Transfers
The system is currently programmed to use just one drive (8) which on my system is an SD card. On VICE you could use a CMD-HD image (.DHD). Either way, you can have multiple partitions on the same drive. For the default C code, it expects:
- Partition 1 : the system drive that includes all the programs and text files, as well as the User Log
- Partition 2 : the Downloads area
- Partition 3 : the Uploads area

Note that Partition 1 can be referred to as Partition 0 as well (so @0: and @1: do the same thing).

When a user uploads a file to the BBS, the file is saved in Partition 3, and the 4-line file description the user provides gets appended to a file called "uploads" which is an SEQ file on Partition 1. You can then manually validate each uploaded file, and if approved copy it from Partition 3 to Partition 2 and then manually edit the filelist SEQ file to be updated with the new file description.  

For example, to copy a file called "test" from Partition 3 to Partition 2...
```
OPEN15,8,15,"C2:TEST=3:TEST":CLOSE15   < Commodore BASIC
@C2:TEST=3:TEST                        < JiffyDOS
```
Likely if file uploads and downloads become popular, I will add extra features like categories, tracking who uploaded the file, the date the file was uploaded, etc. For now, this is an extremely basic method of managing files.

## Who To Contact?
If you have any questions about this project, please email sysop@deepskies.com and we'll do our best to help you out! Thanks for checking this project out!

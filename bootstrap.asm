ADDRESS	CODE	    COMMENT
A000	LDA #$01	load 01 into AC (logical)
A002	LDX #$08	load 08 into XR (device, 08 = disk)
A004	LDY #$03	load 03 into YR (command)
A006	JSR $FFBA	call SETLFS
A009	LDA $FD	    load value from FD (253) which contains our switch value
A00B	CMP #$01	compare AC to 00
A00D	BEQ $A024	if one, branch to BBS
A00F	CMP #$02	compare AC to 01
A011	BEQ $A02D	if two, branch to GAMES
A013	CMP #$03	compare AC to 03
A015	BEQ $A036	if three, branch to EDITOR
A017	CMP #$04	compare AC to 04
A019	BEQ $A03F	if four, branch to FILES
A01B	CMP #$05	compare AC to 05
A01D	BEQ $A048	if five, branch to FUTURE
A01F	CMP #$06	compare AC to 06
A021	BEQ $A051	if six, branch to SYSOP
A023	RTS 	    otherwise return from subroutine
A024	LDA #$03	load length of BBS into AC
A026	LDX #$E2	load lo byte of BBS into XR
A028	LDY #$A0	load hi byte of BBS into YR
A02A	JMP $A057	jump to BOOTSTRAP
A02D	LDA #$05	load length of GAMES into AC
A02F	LDX #$E5	load lo byte of GAMES into XR
A031	LDY #$A0	load hi byte of GAMES into YR
A033	JMP $A057	jump to BOOTSTRAP
A036	LDA #$06	load length of EDITOR into AC
A038	LDX #$EA	load lo byte of EDITOR into XR
A03A	LDY #$A0	load hi byte of EDITOR into YR
A03C	JMP $A057	jump to BOOTSTRAP
A03F	LDA #$05	load length of FILES into AC
A041	LDX #$F0	load lo byte of FILES into XR
A043	LDY #$A0	load hi byte of FILES into YR
A045	JMP $A057	jump to BOOTSTRAP
A048	LDA #$06	load length of FUTURE into AC
A04A	LDX #$F5	load lo byte of FUTURE into XR
A04C	LDY #$A0	load hi byte of FUTURE into YR
A04E	JMP $A057	jump to BOOTSTRAP
A051	LDA #$05	load length of SYSOP into AC
A053	LDX #$FB	load lo byte of SYSOP into XR
A055	LDY #$A0	load hi byte of SYSOP into YR
A057	JSR $FFBD	call SETNAM
A05A	LDA #$00	load zero into AC = LOAD
A05C	JSR $FFD5	call LOAD
A05F	JMP $120D	jump to program (good luck!)

VARIABLES				
START	END	    NAME	LEN	VALUE
A0E2	A0E4	BBS	    3	BBS
A0E5	A0E9	GAMES	5	GAMES
A0EA	A0EF	EDITOR	6	EDITOR
A0F0	A0F4	FILES	5	FILES
A0F5	A0FA	FUTURE	6	FUTURE
A0FB	A0FF	SYSOP	5	SYSOP

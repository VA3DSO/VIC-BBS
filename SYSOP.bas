    0 gosub1000:poke36879,8:print"{clr}{cyn}SYSOP UTILITY{down}"+chr$(14):gu=0
    1 print"{rvon}L{rvof} List All Users"
    2 print"{rvon}E{rvof} Edit a User"
    3 print"{rvon}A{rvof} Add a User"
    4 print"{rvon}D{rvof} Delete a User"
    5 print"{rvon}G{rvof} Generate User List":print"{rvon}B{rvof} Backup User Log":print"{rvon}R{rvof} Restore User Log"
    6 print"{rvon}S{rvof} Display Stats"
    7 print"{rvon}F{rvof} Fix Stats File"
    8 print"{rvon}C{rvof} Create User Log"
    9 print"{down}{rvon}X{rvof} Exit"
   10 print"{down}Command? "
   15 geta$:ifa$=""then15
   20 ifa$="c"then100
   21 ifa$="f"then500
   22 ifa$="l"then200
   23 ifa$="e"then300
   24 ifa$="a"then400
   25 ifa$="x"then900
   26 ifa$="s"then600
   27 ifa$="d"then1400
   28 ifa$="g"then1500
   29 ifa$="b"then1600
   30 ifa$="r"then1700
   99 print"{down}?Bad Command!":goto10
  100 print"{clr}Create User Log{down}"
  105 print"WARNING! Are you sure?"
  106 geta$:ifa$=""then106
  107 ifa$="y"then110
  109 goto0
  110 print"Creating User{$a0}Log...";
  112 open15,8,15,"s0:users":close15
  115 open15,8,15
  116 open1,8,2,"users,l,"+chr$(76)
  117 nl$=chr$(255)
  119 forz=1to255
  120 print#15,"p"+chr$(98)+chr$(z)+chr$(0)+chr$(1)
  122 rc$=nl$+"ULL        NULL      NULL{$a0}USER           NULLWHERE      0----------------"
  125 print#1,rc$
  126 print"{home}{down}{down}{down}{down}{down}{down}      {left}{left}{left}{left}{left}{left}";str$(int(z/255*1000)/10);"%"
  128 nextz
  129 close1
  130 input#15,e,e$:print"{down}Status:";e$:close15
  140 print"{down}Creating SYSOP record"
  145 open15,8,15
  150 open1,8,2,"users,l,"+chr$(76)
  155 print#15,"p"+chr$(98)+chr$(1)+chr$(0)+chr$(1)
  160 print#1,"SYSOP       SYSOP     SYSOP               NOWHERE        6    0    0800101"
  161 close1
  165 input#15,e,e$:print"{down}Status:";e$:close15
  190 print"{down}Updating stats...":gosub1100
  197 print"{down}Press any key..."
  198 geta$:ifa$=""then198
  199 goto 0
  200 rem * list users
  205 print"{clr}Listing";nu;"Users{down}"
  210 open15,8,15
  220 open1,8,2,"users,l,"+chr$(76)
  225 sl=1
  230 cu=1:vu=1
  240 print#15,"p"+chr$(98)+chr$(cu)+chr$(0)+chr$(1)
  250 input#1,rc$
  255 ifleft$(rc$,1)=chr$(255) then 270
  260 printcu;" ";left$(rc$,12)
  263 ifgu=1thenprint#3,str$(cu)+" "+left$(rc$,12)
  265 sl=sl+1:ifsl=21thengosub1300:sl=1
  266 vu=vu+1:ifvu>nuthen280
  270 cu=cu+1:ifcu>255then280
  275 goto240
  280 close1:close15
  285 ifgu=1thenclose3:gu=0
  297 print"{down}Press any key..."
  298 geta$:ifa$=""then298
  299 goto 0
  300 rem * edit user
  305 print"{clr}Edit{$a0}User{down}":input"User ID";id:print
  310 open15,8,15:open1,8,2,"users,l,"+chr$(76)
  315 print#15,"p"+chr$(98)+chr$(id)+chr$(0)+chr$(1):rc$="":fory=1to74:get#1,a$
  320 rc$=rc$+a$:nexty:close1:close15
  321 un$=left$(rc$,12)
  322 pw$=mid$(rc$,13,10)
  323 rn$=mid$(rc$,23,20)
  324 fr$=mid$(rc$,43,15)
  325 sl$=mid$(rc$,58,1)
  326 uc$=mid$(rc$,59,5)
  327 ur$=mid$(rc$,64,5)
  328 ul$=mid$(rc$,69,6)
  330 print"{down}Username:":printun$:print">";:mc=12:gosub800:iflen(i$)>0thenun$=i$
  331 print"{down}Password:":printpw$:print">";:mc=10:gosub800:iflen(i$)>0thenpw$=i$
  332 print"{down}Real Name:":printrn$:print">";:mc=20:gosub800:iflen(i$)>0thenrn$=i$
  333 print"{down}From:":printfr$:print">";:mc=15:gosub800:iflen(i$)>0thenfr$=i$
  334 print"{down}Security Level:":print sl$:print">";:mc=1:gosub800:iflen(i$)>0thensl$=i$
  335 print"{down}Number of Calls:":print uc$:print">";:mc=5:gosub800:iflen(i$)>0thenuc$=i$
  336 print"{down}Last Message Read:":print ur$:print">";:mc=5:gosub800:iflen(i$)>0thenur$=i$
  337 print"{down}Last Logon:":print ul$:print">";:mc=6:gosub800:iflen(i$)>0thenul$=i$
  340 print"{clr}UN:";un$
  341 print"PW:";pw$
  342 print"RN:";rn$
  343 print"FR:";fr$
  344 print"SL:";sl$
  345 print"NC:";uc$
  346 print"LR:";ur$
  347 print"LC:";ul$
  350 print"{down}ARE{$a0}YOU{$a0}SURE? (Y/N)"
  351 geta$:ifa$=""then351
  355 ifa$<>"y"then399
  356 print"{down}Saving..."
  360 rc$=un$+pw$+rn$+fr$+sl$+uc$+ur$+ul$
  361 iflen(rc$)<>74thenprint"{down}NOT{$a0}76 CHARS!":gosub1300:goto330
  370 open15,8,15:open1,8,2,"users,l,"+chr$(76)
  375 print#15,"p"+chr$(98)+chr$(id)+chr$(0)+chr$(1):print#1,rc$:close1
  380 input#15,e,e$:print"{down}Status:";e$:close15
  390 gosub 1300
  399 goto0
  400 rem * add user
  405 print"{clr}Add User{down}":gosub1200:print"Username:":print">";:mc=12:gosub800:un$=i$
  410 print"{down}Password:":print">";:mc=10:gosub800:pw$=i$
  411 print"{down}Real Name:":print">";:mc=20:gosub800:rn$=i$
  412 print"{down}From:":print">";:mc=15:gosub800:fr$=i$
  413 print"{down}Security Level:":print">";:mc=1:gosub800:sl$=i$
  414 uc$="    0"
  415 ur$="    0"
  416 ul$="800101"
  419 print"{clr}ID:";id
  420 print"UN:";un$
  421 print"PW:";pw$
  422 print"RN:";rn$
  423 print"FR:";fr$
  424 print"SL:";sl$
  425 print"NC:";uc$
  426 print"LR:";ur$
  427 print"LC:";ul$
  430 print"{down}ARE{$a0}YOU{$a0}SURE? (Y/N)"
  431 geta$:ifa$=""then431
  435 ifa$<>"y"then499
  440 rc$=un$+pw$+rn$+fr$+sl$+uc$+ur$+ul$
  441 iflen(rc$)<>74thenprint"{down}NOT{$a0}76 CHARS!":gosub1300:goto498
  450 open15,8,15:open1,8,2,"users,l,"+chr$(76)
  455 print#15,"p"+chr$(98)+chr$(id)+chr$(0)+chr$(1):print#1,rc$:close1
  460 input#15,e,e$:print"{down}Status:";e$:close15
  470 nu=nu+1:gosub1100
  498 gosub 1300
  499 goto0
  500 rem * fix stats
  510 print"{clr}Fix Stats{down}"
  515 print"Last Caller:":printlc$:inputz$:ifz$<>""thenlc$=z$
  516 z$=""
  520 print"Number of Calls:":printnc:inputz$:ifz$<>""thennc=val(z$)
  521 z$=""
  530 print"Number of Users:":printnu:inputz$:ifz$<>""thennu=val(z$)
  531 z$=""
  540 print"Number of Msgs:":printnm:inputz$:ifz$<>""thennm=val(z$)
  541 z$=""
  550 print"Number of Bulletins:":printnb:inputz$:ifz$<>""thennb=val(z$)
  551 z$=""
  555 print"{down}Saving..."
  560 gosub 1100
  598 gosub1300
  599 goto0
  600 rem * disp stats
  601 print"{clr}Current{$a0}Stats:{down}"
  605 print"  LAST:";lc$
  610 print"#CALLS:";nc
  620 print"#USERS:";nu
  630 print" #MSGS:";nm
  640 print"#BULLS:";nb
  690 gosub1300
  699 goto 0
  700 rem * pad field
  710 sp$="                      "
  715 iflen(i$)<1then799
  720 i$=left$(i$+sp$,mc)
  799 return
  800 rem * getline
  803 sp$="                      "
  805 i$="":c$="{CBM-P}":d$=chr$(20):printc$;
  810 geta$:ifa$=""then810
  815 ifa$=chr$(13)thenprintd$:gosub700:goto899
  820 ifa$=d$andlen(i$)>0thenprintd$;d$;c$;:i$=left$(i$,len(i$)-1):goto810
  825 ifa$=d$then810
  830 iflen(i$)=mcthen810
  890 i$=i$+a$:printd$;a$;c$;:goto810
  899 return
  900 end
  999 end
 1000 rem * load stats
 1005 print"{down}Loading stats..."
 1010 open3,8,3,"stats,s,r":input#3,lc$,nc,nu,nm,nb
 1020 close3
 1099 return
 1100 rem * updt stats
 1105 open15,8,15
 1110 open3,8,3,"@0:stats,s,w"
 1120 print#3,lc$
 1130 print#3,nc
 1140 print#3,nu
 1150 print#3,nm
 1160 print#3,nb
 1170 close3
 1180 input#15,e,e$
 1190 print"{down}Status:";e$:close15
 1199 return
 1200 rem * find id
 1210 open15,8,15
 1220 open1,8,2,"users,l,"+chr$(76)
 1230 id=1
 1235 print#15,"p"+chr$(98)+chr$(id)+chr$(0)+chr$(1)
 1240 get#1,a$:ifa$=chr$(255)then1290
 1250 id=id+1:ifid=256thenprint"{down}USER{$a0}LOG{$a0}FULL!":goto1290
 1260 goto1235
 1290 close1:close15
 1299 return
 1300 print"{down}Press any key...";
 1310 geta$:ifa$=""then1310
 1320 print
 1399 return
 1400 rem * del user
 1405 print"{clr}Delete A User"
 1410 input"User ID";id
 1415 open15,8,15
 1416 open1,8,2,"users,l,"+chr$(76)
 1420 print#15,"p"+chr$(98)+chr$(id)+chr$(0)+chr$(1)
 1425 rc$="":fork=1to74:get#1,a$:rc$=rc$+a$:nextk:close1:close15
 1430 print"{down}";rc$:print"{down}Are you sure?{$a0}(Y/N)"
 1435 geta$:ifa$=""then1435
 1440 ifa$<>"y"then1499
 1445 print"{down}Deleting..."
 1450 open15,8,15
 1451 open1,8,2,"users,l,"+chr$(76)
 1452 print#15,"p"+chr$(98)+chr$(id)+chr$(0)+chr$(1)
 1453 rc$=chr$(255)+right$(rc$,73)
 1455 print#1,rc$:close1:input#15,e,e$:print"{down}Status:";e$:close15
 1460 nu=nu-1:gosub1100
 1499 goto0
 1500 rem * user list
 1505 print"{clr}Generating User List"
 1510 open3,8,3,"@0:userlog,s,w"
 1520 gu=1:goto210
 1600 rem * backup
 1605 print"{clr}Backup{down}"
 1610 input"Drive to use";dr
 1615 print"{down}Backing up";nu:print"records...{down}"
 1620 open15,8,15
 1625 open3,dr,3,"@0:users-bak,s,w"
 1626 open1,8,2,"@0:users,l,"+chr$(76)
 1630 z=1
 1635 print#15,"p"+chr$(98)+chr$(z)+chr$(0)+chr$(1):rc$=""
 1640 fory=1to74:get#1,a$:rc$=rc$+a$:nexty
 1643 ifleft$(rc$,1)=chr$(255)then1648
 1645 print#3,rc$:print"{home}{down}{down}{down}{down}{down}{down}";z:z=z+1:ifz>nuthen1650
 1648 goto1635
 1650 close1:close3
 1655 input#15,e,e$:print"{down}Status:";e$:close15
 1698 gosub1300
 1699 goto0
 1700 rem * retore
 1705 print"{clr}Restore{down}":print"THIS{$a0}IS{$a0}DESTRUCTIVE!{down}":print"Are you sure? (Y/N)"
 1706 geta$:ifa$=""then1706
 1707 ifa$<>"y"then1799
 1708 print"{down}Have you run CREATE   first? (Y/N)"
 1709 geta$:ifa$=""then1709
 1710 ifa$<>"y"then1799
 1711 input"{down}Drive to use";dr:print"{down}Restoring..."
 1713 open15,8,15
 1715 open3,dr,3,"users-bak,s,r":x=0:rc$=""
 1716 open1,8,2,"users,l,"+chr$(76)
 1720 get#3,a$
 1725 rc$=rc$+a$
 1730 ifa$=chr$(13)thenx=x+1:printx;left$(rc$,12):gosub1750:rc$=""
 1740 ifst=0then1720
 1745 goto 1790
 1750 rem * add rec
 1755 print#15,"p"+chr$(98)+chr$(x)+chr$(0)+chr$(1):print#1,rc$
 1789 return
 1790 close1:close3
 1795 input#15,e,e$:print"{down}Status:";e$:close15
 1796 nu=x:gosub1100
 1798 gosub1300
 1799 goto0
 1989 return

    0 rem *** vic-bbs ***
    1 open5,2,0,chr$(8):poke169,192
    2 poke36879,10:print"{wht}";chr$(14);
    3 f$="":a$="":o$=""
    4 gosub3300:gosub3000
    5 rem ***************
    6 rem *             *
    7 rem *  main loop  *
    8 rem *             *
    9 rem ***************
   10 gosub 100:ifsd=1thengoto99
   11 ifcd=0then98
   20 gosub 1000
   30 gosub 200:ifcd=0then98
   40 gosub 1200
   50 gosub 2000
   98 gosub1400:close5:run
   99 gosub1400:close5:end
  100 rem * answer    *
  101 print"{clr}{rvon}      VIC-BBS V1      {rvof}{down}"
  102 print"Last Caller:":printlc$:print
  103 print"    Calls:";nc
  104 print"    Users:";nu
  105 print" Messages:";nm
  106 print"Bulletins:";nb
  107 print"{down}Waiting..."
  108 print"{down}(Press {rvon} F1 {rvof} to exit)"
  110 geta$:ifa$="{f1}"thensd=1:goto199
  112 ifa$=" "thenprint"{red}":goto100
  115 if(peek(rs)and8)=0then110
  120 fort=1to255:get#5,z$:nextt:cd=1
  130 o$=cr$+"Press RETURN:":sysa(1)
  131 poke254,1:sysa(3)
  132 ifpeek(780)=255thencd=0:goto199
  133 ifpeek(780)=13then140
  139 goto131
  140 o$=cr$+"."+chr$(7):sysa(1):gosub750
  141 o$="."+chr$(7):sysa(1):gosub750
  145 o$=cr$:sysa(1)
  199 return
  200 rem * logon     *
  205 lt=0
  206 lt=lt+1:iflt>3theno$=cr$+"Too many tries!"+cr$:sysa(1):gosub1400:goto299
  210 o$=cr$+"{wht}Enter User ID or NEW:"+cr$+"(? to list all IDs)"+cr$+">":sysa(1):mc=3:id=0
  220 poke254,1
  230 gosub400:o$=cr$:sysa(1):gosub1600
  240 ifcd=0then299
  250 ifleft$(i$,3)="NEW"orleft$(i$,3)="new"thengosub300:goto299
  260 ifleft$(i$,1)="?"thengosub1100:goto210
  270 ifval(i$)>0andval(i$)<=255thengosub3200
  280 ifid=0theno$=cr$+"Bad ID or password!"+cr$:sysa(1):goto206
  285 ifval(sl$)<1thenf$="banned,s,r":sysa(2):cd=0:goto299
  289 rem * user stats
  290 o$=cr$+"{clr}Hi "+rn$+cr$+cr$:sysa(1)
  291 o$="USERNAME:"+un$+cr$:sysa(1)
  292 o$="LST USER:"+lc$+cr$:sysa(1)
  294 o$="LST CALL:"+ul$+cr$:sysa(1)
  295 z$=str$(val(uc$)):o$=" # CALLS:"+right$(z$,len(z$)-1)+cr$:sysa(1)
  296 z$=str$(id):o$=" USER ID:"+right$(z$,len(z$)-1)+cr$:sysa(1)
  297 o$="SECURITY:"+sl$+cr$:sysa(1):z$=str$(nm-val(ur$))
  298 o$="NEW MSGS:"+right$(z$,len(z$)-1)+cr$:sysa(1):gosub1800
  299 return
  300 rem * new user  *
  310 f$="newuser1,s,r":sysa(2)
  320 gosub 1300
  325 ifcd=0then399
  330 o$="{clr}IMPORTANT!"+cr$+cr$+"Write down your User"+cr$+"ID and Password:"+cr$+cr$
  335 sysa(1)
  340 o$="User ID ="+str$(id)+cr$:sysa(1)
  350 o$="Password= "+pw$+cr$:sysa(1)
  360 gosub 1800
  365 o$="{clr}":sysa(1)
  370 f$="newuser2,s,r":sysa(2)
  380 gosub 1800
  399 return
  400 rem * main i/o  *
  401 i$="":np=0:iflen(fu$)>0theni$=fu$:fu$="":o$=cr$+i$:sysa(1)
  405 sysa(3):x=peek(780):ifx>0thena$=chr$(x)
  406 np=np+peek(781)
  410 ifx=13theno$=cr$:sysa(1):goto499
  411 ifx=20thengosub1500:goto405
  412 ifx=255thencd=0:goto499
  415 iflen(i$)=mc+npandww=0then405
  420 i$=i$+a$
  425 ifma=1theno$="*"
  430 ifma=0theno$=a$
  435 sysa(1)
  436 iflen(i$)>=mc+npandww=1then440
  439 goto405
  440 rem * word wrap
  445 j=len(i$):ifj=0then499
  447 a$=mid$(i$,j,1):ifa$=" "andj<len(i$)then490
  450 fu$=a$+fu$:o$=chr$(20):sysa(1)
  453 iflen(fu$)>13theno$=fu$+cr$:sysa(1):fu$="":goto499
  455 j=j-1:ifj=0then499
  460 goto 447
  490 i$=left$(i$,len(i$)-len(fu$))
  499 return
  500 rem * get rc$   *
  510 rc$=""
  520 forzz=1to74:get#1,z$:rc$=rc$+z$:nextzz
  599 return
  600 rem * logoff    *
  610 o$=cr$+"LOGOFF"+cr$:sysa(1):gosub1850
  620 ifa$=chr$(255)thencd=0
  630 ifa$="y"thenf$="logoff,s,r":sysa(2):cd=0
  699 return
  700 rem * pause     *
  710 fort=1to666:next
  749 return
  750 rem * beep      *
  760 poke36876,195
  765 poke36878,10
  770 fort=1to99:next
  775 poke36878,0
  780 fort=1to567:next
  799 return
  800 rem * printline *
  899 return
  900 rem * debug     *
  910 o$="{clr}{wht}DEBUG{$a0}SCREEN"+cr$+cr$
  911 o$="UN$= "+un$+cr$:sysa(1)
  912 o$="ID ="+str$(id)+cr$:sysa(1)
  913 o$="SL$= "+sl$+cr$:sysa(1)
  914 o$="UC$= "+uc$+cr$:sysa(1)
  915 o$="UR$= "+ur$+cr$:sysa(1)
  916 o$="CD ="+str$(cd)+cr$:sysa(1)
  917 o$="DA$= "+da$+cr$:sysa(1)
  918 o$="LC$= "+lc$+cr$:sysa(1):gosub1800
  919 o$=cr$+"MA ="+str$(ma)+cr$:sysa(1)
  920 o$="NB ="+str$(nb)+cr$:sysa(1)
  921 o$="NC ="+str$(nc)+cr$:sysa(1)
  922 o$="NM ="+str$(nm)+cr$:sysa(1)
  923 o$="NU ="+str$(nu)+cr$:sysa(1)
  924 o$="RS ="+str$(rs)+cr$:sysa(1)
  925 o$="RT ="+str$(rt)+cr$:sysa(1)
  926 o$="SD ="+str$(sd)+cr$:sysa(1)
  927 o$="TM ="+str$(tm)+cr$:sysa(1)
  928 o$="TU ="+str$(tu)+cr$:sysa(1):gosub1800:o$=cr$:sysa(1)
  929 o$="PW$= "+pw$+cr$:sysa(1)
  930 forz=1to6
  931 o$="TL("+str$(z)+")="+str$(tl(z))+cr$:sysa(1)
  932 nextz
  998 gosub1800
  999 return
 1000 rem * banner   *
 1010 f$="banner,s,r"
 1020 sysa(2)
 1099 return
 1100 rem * usr list *
 1110 o$=cr$+"User Listing"+cr$+cr$:sysa(1)
 1120 f$="userlog,s,r":sysa(2)
 1130 gosub1800
 1199 return
 1200 rem * motd     *
 1210 f$="motd,s,r"
 1220 sysa(2)
 1225 gosub1800
 1230 f$="help,s,r"
 1240 sysa(2)
 1299 return
 1300 rem * applictn *
 1305 id=0:un$="":pw$="":rn$="":fr$="":sl$="3":co$="    ":close1:close15
 1310 o$="{yel}Handle?"+cr$+">{wht}":sysa(1):mc=12:gosub400:gosub1600:un$=i$:ifcd=0then1399
 1311 ifun$=""then1310
 1312 o$=cr$+"Checking..."+cr$:sysa(1)
 1315 open15,8,15:open1,8,2,"users,l,"+chr$(76):y=1:vu=1
 1317 print#15,"p"+chr$(98)+chr$(y)+chr$(0)+chr$(1):input#1,rc$:i$=left$(rc$,12)
 1318 gosub1600:ifi$=un$theno$=cr$+"{red}Handle aleady taken!{wht}"+cr$+cr$:sysa(1):goto1305
 1319 ifleft$(rc$,1)<>chr$(255)thenvu=vu+1:ifvu>nuthen1325
 1320 o$=".":sysa(1):y=y+1:ify<255then1317
 1325 close1:close15
 1330 o$=cr$+"{yel}Password?"+cr$+">{wht}":sysa(1):mc=10:gosub400:gosub1600:pw$=i$:ifcd=0then1399
 1331 o$=cr$+"{yel}Real name?"+cr$+">{wht}":sysa(1):mc=20:gosub400:rn$=i$:ifcd=0then1399
 1332 o$=cr$+"{yel}From?"+cr$+">{wht}":sysa(1):mc=15:gosub400:fr$=i$:ifcd=0then1399
 1335 ifun$=""orpw$=""orrn$=""orfr$=""theno$=cr$+"Missing fields!"+cr$:sysa(1):goto1300
 1340 o$="{clr}{yel}You entered..."+cr$+cr$:sysa(1):o$="{yel}Handle:"+cr$+"{wht}"+un$+cr$:sysa(1)
 1341 o$=cr$+"{yel}Password:"+cr$+"{wht}"+pw$+cr$:sysa(1)
 1342 o$=cr$+"{yel}Real name:"+cr$+"{wht}"+rn$+cr$:sysa(1)
 1343 o$=cr$+"{yel}From:"+cr$+"{wht}"+fr$+cr$:sysa(1)
 1350 gosub1850:ifa$="n"then1305
 1351 ifcd=0then1399
 1360 gosub1900:ifid>255thenf$="logfull,s,r":sysa(2):cd=0:id=0:goto1399
 1365 o$=cr$+"Saving..."+cr$:sysa(1)
 1366 sp$="                      "
 1370 un$=left$(un$+sp$,12)
 1371 pw$=left$(pw$+sp$,10)
 1372 rn$=left$(rn$+sp$,20)
 1373 fr$=left$(fr$+sp$,15)
 1375 rc$=un$+pw$+rn$+fr$+"3    0    0800101"
 1380 iflen(rc$)<>74theno$=cr$+"{rvon}ERROR!{rvof}"+cr$:sysa(1):cd=0:id=0:goto1399
 1385 open15,8,15:open1,8,2,"users,l,"+chr$(76)
 1386 print#15,"p"+chr$(98)+chr$(id)+chr$(0)+chr$(1):print#1,rc$:close1
 1387 input#15,e,e$:o$="{down}Status:"+e$+cr$:sysa(1):close15
 1390 nu=nu+1:gosub3100
 1395 ad=0:gosub3450:tu=tm+tl(val(sl$))*60:iftu>86400thentu=tu-86400:ad=1
 1399 return
 1400 rem * hang up  *
 1405 nc=nc+1:iflen(un$)>0thenlc$=un$
 1410 gosub700
 1420 poke rs,151
 1430 gosub700
 1440 poke rs,peek(rs)and32
 1450 gosub700
 1460 cd=0
 1470 gosub3100
 1475 gosub3500
 1479 return
 1480 gosub700
 1485 poke rs,peek(rs)and32
 1490 gosub700
 1495 cd=0
 1499 return
 1500 rem * del char *
 1505 cc=0
 1510 iflen(i$)<1then1599
 1512 n=asc(right$(i$,1)+chr$(0))
 1513 ifn=5orn=18orn=28orn=30orn=31orn=146orn=156orn=158orn=159thennp=np-1:cc=1
 1514 ifnp<0thennp=0
 1520 i$=left$(i$,len(i$)-1)
 1525 ifcc=1then1599
 1530 o$=chr$(20)
 1540 sysa(1)
 1599 return
 1600 rem * toupper  *
 1603 iflen(i$)<1then1699
 1604 tr=0
 1605 fork=len(i$)to1step-1:x$=mid$(i$,k,1)
 1607 ifasc(x$)=32andtr=0andlen(i$)>1theni$=left$(i$,len(i$)-1)
 1608 ifasc(x$)<>32thentr=1
 1609 ifx$=","thenx$="-":gosub1660
 1610 ifasc(x$)>64andasc(x$)<91thengosub1650
 1620 next k:goto1699
 1650 x$=chr$(asc(x$)+128):i$=left$(i$,k-1)+x$+right$(i$,len(i$)-k)
 1655 return
 1660 i$=left$(i$,k-1)+x$+right$(i$,len(i$)-k)
 1669 return
 1699 return
 1700 rem * show time*
 1710 gosub3450
 1720 ifad=1thenx=tu-(tm-86400)
 1721 ifad=0thenx=tu-tm
 1722 x=x/60
 1725 x1=x-int(x):x1=int(x1*60):z1$=str$(x1):z1$=right$(z1$,len(z1$)-1)
 1730 x=int(x):z$=str$(x):z$=right$(z$,len(z$)-1)
 1735 iflen(z1$)=1thenz1$="0"+z1$
 1740 o$=cr$+z$+" mins "+z1$+" secs left"+cr$
 1750 sysa(1)
 1799 return
 1800 rem * press key*
 1810 o$=cr$+"Press any key...":sysa(1)
 1820 poke254,1
 1830 sysa(3)
 1849 return
 1850 rem * you sure *
 1860 o$=cr$+"Are you sure? (Y/N)":sysa(1)
 1865 poke254,1:sysa(3):a$=chr$(peek(780)):ifa$="y"ora$="Y"thena$="y":goto1898
 1870 ifa$="n"ora$="N"thena$="n":goto1898
 1880 ifa$=chr$(255)thencd=0:goto1899
 1897 goto1865
 1898 o$=a$+cr$:sysa(1)
 1899 return
 1900 rem * find id  *
 1910 open15,8,15
 1920 open1,8,2,"users,l,"+chr$(76)
 1930 id=1
 1935 print#15,"p"+chr$(98)+chr$(id)+chr$(0)+chr$(1)
 1940 get#1,a$:ifa$=chr$(255)then1990
 1950 id=id+1:ifid>255theno$=cr$+"{rvon}USER{$a0}LOG{$a0}FULL!{rvof}"+cr$:sysa(1):goto1990
 1960 goto1935
 1990 close1:close15
 1999 return
 2000 rem * main mnu *
 2001 o$=cr$+"{wht}Command:":sysa(1):poke254,1:sysa(3):a$=chr$(peek(780)):o$=a$+cr$:sysa(1)
 2002 ifa$=">"thengosub900:goto2001
 2003 gosub3450:iftm>tuandad=0thenf$="timeup,s,r":sysa(2):cd=0:goto2099
 2004 ifa$="t"ora$="T"thengosub1700:goto2001
 2005 ifa$="v"ora$="V"thengosub1100:goto2001
 2010 ifa$="b"ora$="B"thengosub4000:goto2001
 2011 ifa$="r"ora$="R"thengosub4100:goto2001
 2012 ifa$="p"ora$="P"thengosub4200:goto2001
 2013 ifa$="l"ora$="L"thengosub4300:goto2001
 2014 ifa$="d"ora$="D"thengosub4400:goto2001
 2015 ifa$="u"ora$="U"thengosub4500:goto2001
 2016 ifa$="i"ora$="U"thengosub4600:goto2001
 2017 ifa$="s"ora$="S"thengosub4700:goto2001
 2018 ifa$="c"ora$="C"thengosub4800:goto2001
 2019 ifa$=">"thengosub900:goto2001
 2020 ifa$="z"ora$="Z"thengosub6000:goto2001
 2094 ifa$=chr$(255)thencd=0:goto2099
 2095 ifa$="?"ora$="h"ora$="H"thenf$="help,s,r":sysa(2):goto2001
 2096 ifa$="o"ora$="O"thengosub600:ifcd=1then2001
 2097 ifcd=0then2099
 2098 o$=cr$+"Press ? for Help!"+cr$:sysa(1):goto2001
 2099 return
 3000 rem * ld stats *
 3005 print"{down}Loading stats..."
 3010 open3,8,3,"stats,s,r":input#3,lc$,f1$,f2$,f3$,f4$:close3
 3020 nc=val(f1$)
 3021 nu=val(f2$)
 3022 nm=val(f3$)
 3023 nb=val(f4$)
 3030 ifnu=0thennu=1
 3099 return
 3100 rem * sv stats *
 3105 print"{down}Saving stats..."
 3107 open15,8,15
 3110 open3,8,3,"@0:stats,s,w"
 3120 print#3,lc$
 3130 print#3,str$(nc)
 3140 print#3,str$(nu)
 3150 print#3,str$(nm)
 3160 print#3,str$(nb)
 3170 close3
 3180 input#15,e,e$:print"{down}Status:";e$:close15
 3199 return
 3200 rem * validate *
 3203 q=val(i$)
 3205 o$=cr$+"Enter Your Password:"+cr$+">":sysa(1):mc=10:ma=1:poke254,1:gosub400:ma=0
 3210 gosub1600
 3220 open15,8,15
 3222 o$=cr$+"Checking..."+cr$+cr$:sysa(1)
 3225 open1,8,2,"users,l,"+chr$(76)
 3230 print#15,"p"+chr$(98)+chr$(q)+chr$(0)+chr$(1)
 3240 gosub500
 3250 close1
 3260 close15
 3265 ifleft$(rc$,1)=chr$(255)then3299
 3270 pw$=mid$(rc$,13,10):forx=len(i$)to9:i$=i$+" ":nextx
 3272 i$=left$(i$,10)
 3275 ifpw$<>i$then3299
 3280 id=q
 3281 un$=left$(rc$,12)
 3282 rn$=mid$(rc$,23,20)
 3283 fr$=mid$(rc$,43,15)
 3284 sl$=mid$(rc$,58,1)
 3285 uc$=mid$(rc$,59,5)
 3286 ur$=mid$(rc$,64,5)
 3287 ul$=mid$(rc$,71,2)+"/"+mid$(rc$,73,2)+"/"+mid$(rc$,69,2)
 3288 ad=0:gosub3450:tu=tm+tl(val(sl$))*60:iftu>86400thentu=tu-86400:ad=1
 3299 return
 3300 rem * load ml  *
 3305 ifpeek(1024)=72then3350
 3310 print"{clr}Loading ML..."
 3320 open3,8,3,"bbs.ml,p,r"
 3325 get#3,h1$:get#3,h2$
 3330 fort=1024to1562
 3331 get#3,v$:poket,asc(v$+chr$(0))
 3332 next t
 3340 close3
 3350 dim a(4)
 3355 poke254,0
 3360 a(1)=1048
 3370 a(2)=1078
 3380 a(3)=1335
 3385 a(4)=1504
 3390 sd=0:rs=37136:cr$=chr$(13):iz=0:cd=0:mc=20:ma=0:rt=38912:qr=40960:np=0
 3391 dim tl(6):tl(1)=20:tl(2)=30:tl(3)=60:tl(4)=60:tl(5)=90:tl(6)=90:em=0:ad=0:ww=0
 3399 return
 3400 rem * get date *
 3410 v=peek(rt+9):v$=str$(v):da$=right$(v$,len(v$)-1)+"/"
 3420 v=peek(rt+8):v$=str$(v):da$=da$+right$(v$,len(v$)-1)+"/"
 3430 v=peek(rt+7):v$=str$(v):da$=da$+right$(v$,len(v$)-1)
 3449 return
 3450 rem * get time *
 3455 v=peek(rt+4):tm=v*3600
 3460 v=peek(rt+2):tm=tm+v*60
 3465 v=peek(rt):tm=tm+v
 3470 iftu>tmthenad=0
 3489 return
 3500 rem * updt user
 3505 print"{down}Updating user..."
 3506 ifid<1orid>255thenprint"{down}Skipping!":goto3599
 3510 open15,8,15
 3511 open1,8,2,"users,l,"+chr$(76)
 3512 print#15,"p"+chr$(98)+chr$(id)+chr$(0)+chr$(1)
 3515 uc$=str$(val(uc$)+1):uc$=right$("    "+uc$,5)
 3516 ur$=str$(val(ur$)):ur$=right$("    "+ur$,5)
 3518 rt=38912
 3520 v=peek(rt+9)
 3521 ul$=right$("0"+right$(str$(v),len(str$(v))-1),2)
 3522 v=peek(rt+8)
 3523 ul$=ul$+right$("0"+right$(str$(v),len(str$(v))-1),2)
 3524 v=peek(rt+7)
 3525 ul$=ul$+right$("0"+right$(str$(v),len(str$(v))-1),2)
 3529 rc$=left$(rc$,58)+uc$+ur$+ul$:iflen(rc$)<>74thenclose1:close15:goto3599
 3530 print#15,"p"+chr$(98)+chr$(id)+chr$(0)+chr$(1)
 3540 print#1,rc$:close1
 3550 input#15,e,e$:print"{down}Status:";e$:close15
 3599 return
 4000 rem * bulletins
 4010 f$="bulletins,s,r":sysa(2)
 4015 o$="{wht}Bulletin:":sysa(1):poke254,1:sysa(3):a$=chr$(peek(780)):o$=a$+cr$:sysa(1)
 4016 bl=val(a$)
 4017 gosub3450:iftm>tuandad=0thenf$="timeup,s,r":sysa(2):cd=0:goto4099
 4020 ifa$="x"ora$="X"then4099
 4030 ifbl>=1andbl<=255theno$="{clr}":sysa(1):f$="b "+a$+",s,r":sysa(2):gosub1800:goto4010
 4040 ifa$="?"ora$="h"ora$="H"then4010
 4097 ifa$=chr$(255)thencd=0:goto4099
 4098 o$=cr$+"Press ? for List!"+cr$:sysa(1):goto4015
 4099 return
 4100 rem * read msgs
 4101 ifval(sl$)<2theno$=cr$+"{wht}Sorry, access denied!"+cr$:sysa(1):goto4199
 4102 lm=nm-255:iflm<1thenlm=1
 4103 o$=cr$+"{clr}{wht}Read Messages"+cr$+cr$:sysa(1):nr=val(ur$)
 4105 o$=cr$+"{rvon}RETURN{rvof} - Next{$a0}Msg"+cr$+"{rvon}P{rvof} - Previous Msg"+cr$+"{rvon}#{rvof} - Enter Msg #"+cr$
 4106 sysa(1):o$="{rvon}A{rvof} - Read Again"+cr$+"{rvon}R{rvof} - Reply"+cr$:sysa(1)
 4108 o$="{rvon}X{rvof} - Exit Messages"+cr$:sysa(1)
 4109 o$=cr$+"Low Msg:"+str$(lm)+cr$+"High Msg:"+str$(nm)+cr$:sysa(1)
 4110 am$=str$(nm):am$=right$(am$,len(am$)-1)
 4111 mm$=str$(nr):mm$=right$(mm$,len(mm$)-1):o$=cr$+"{wht}Messages("+mm$+"/"+am$+"):"
 4112 sysa(1):poke254,1:sysa(3):a$=chr$(peek(780))
 4115 o$=a$+cr$:sysa(1):gosub3450:iftm>tuandad=0thenf$="timeup,s,r":sysa(2):cd=0:goto4199
 4120 ifa$=chr$(255)thencd=0:goto4199
 4122 ifa$="?"then4105
 4125 ifa$="x"ora$="X"then4160
 4130 ifa$=chr$(13)thennr=nr+1:goto4150
 4132 ifa$="a"ora$="A"then4150
 4135 ifa$="p"ora$="P"thennr=nr-1:goto4150
 4140 ifa$="#"then4170
 4145 ifa$="r"ora$="R"then4180
 4149 o$=cr$+"Press ? for Help!"+cr$:sysa(1):goto4110
 4150 ifnr<lmthennr=lm:goto4110
 4152 ifnr>nmthennr=nm:goto4110
 4155 o$="{clr}":sysa(1):f$="m"+str$(nr)+",s,r":sysa(2):goto4110
 4160 x=val(ur$):ifnr>xthenur$=right$("     "+str$(nr),5):gosub3500
 4165 goto4199
 4170 o$=cr$+"Enter Msg #:"+cr$+">":sysa(1):gosub400
 4172 ifval(i$)<lmorval(i$)>nmthen4110
 4174 nr=val(i$):goto4150
 4180 o$=cr$+"Just a sec..."+cr$:sysa(1)
 4182 open3,8,3,"m"+str$(nr)+",s,r":forz=1to5:i$="":k=0
 4183 get#3,a$:ifa$=chr$(13)then4187
 4184 i$=i$+a$:k=k+1:ifk>21then4187
 4185 goto4183
 4187 nextz:close3
 4188 ifleft$(i$,3)<>"RE:"theni$="RE:"+i$
 4189 gosub5003:goto4110
 4199 return
 4200 rem * post msg
 4201 ifval(sl$)<3theno$=cr$+"{wht}Sorry, access denied!"+cr$:sysa(1):goto4299
 4250 gosub5000
 4299 return
 4300 rem * list files
 4310 o$="{clr}List Files"+cr$+cr$+"Not implemented yet."+cr$:sysa(1)
 4399 return
 4400 rem * download
 4410 o$="{clr}Download File"+cr$+cr$+"Not implemented yet."+cr$:sysa(1)
 4499 return
 4500 rem * upload
 4510 o$="{clr}Upload a File"+cr$+cr$+"Not implemented yet."+cr$:sysa(1)
 4599 return
 4600 rem * sysinfo
 4610 f$="info,s,r":sysa(2)
 4699 return
 4700 rem * settings
 4710 f$="setmenu,s,r":sysa(2)
 4715 o$="{wht}Setting:":sysa(1):poke254,1:sysa(3):a$=chr$(peek(780)):o$=a$+cr$:sysa(1)
 4717 gosub3450:iftm>tuandad=0thenf$="timeup,s,r":sysa(2):cd=0:goto4799
 4720 ifa$="h"ora$="H"thenf$="syshelp,s,r":sysa(2):gosub1800:goto4710
 4725 ifa$="p"ora$="P"thengosub4900:goto4710
 4796 ifa$=chr$(255)thencd=0:goto4799
 4797 ifa$="x"ora$="X"then4799
 4798 o$=cr$+"Press ? for Help!"+cr$:sysa(1):goto4715
 4799 return
 4800 rem * chat
 4810 o$="{clr}Paging the sysop...":sysa(1)
 4820 zz=1
 4821 gosub750
 4822 poke36879,zz
 4825 geta$:ifa$<>""then4840
 4829 zz=zz+1:ifzz<20then4821
 4830 poke36879,8
 4835 o$=cr$+"Sorry, no answer."+cr$:sysa(1):goto4899
 4840 o$="{clr}Entering Chat Mode!"+cr$+cr$+"Press {rvon} F3 {rvof} to exit."+cr$+cr$:sysa(1)
 4845 poke36879,8
 4850 sysa(4)
 4860 o$=cr$+"Exiting chat..."+cr$:sysa(1)
 4899 return
 4900 rem * chg pswd
 4910 o$="{clr}{wht}Change{$a0}Password"+cr$+cr$:sysa(1)
 4915 o$="Current Password:"+cr$+">"+pw$+cr$+cr$:sysa(1)
 4920 o$="{yel}Enter NEW{$a0}Password"+cr$+"or leave blank to"+cr$+"abort."+cr$+">":sysa(1)
 4925 mc=10:poke254,1:gosub400:o$=cr$:sysa(1):gosub1600
 4926 ifi$=""then4999
 4930 o$=cr$+i$+cr$:sysa(1):gosub1850:ifa$<>"y"then4999
 4935 np$=left$(i$+"          ",10)
 4940 open15,8,15:open1,8,2,"users,l,"+chr$(76)
 4945 print#15,"p"+chr$(98)+chr$(id)+chr$(0)+chr$(1):gosub500
 4950 rc$=left$(rc$,12)+np$+right$(rc$,52)
 4955 iflen(rc$)<>74theno$=cr$+cr$+"{rvon}{red}LEN{$a0}ERROR!{rvof}{wht}"+cr$+"["+rc$+"]"+cr$:sysa(1):goto4998
 4960 print#15,"p"+chr$(98)+chr$(id)+chr$(0)+chr$(1)
 4965 print#1,rc$
 4970 input#15,e,e$:o$=cr$+"{wht}Status:"+e$+cr$:sysa(1)
 4975 pw$=np$
 4998 close1:close15:gosub1800
 4999 return
 5000 rem * editor   *
 5001 o$="{clr}{wht}New Message"+cr$+cr$:sysa(1):o$="Subject?"+cr$+">":sysa(1):poke254,1:mc=18
 5002 gosub400:ifi$=""then5099
 5003 su$=i$:o$=cr$+"Hang on..."+cr$:sysa(1):gosub3400:qx=0:ww=1
 5004 mm$=str$(nm+1):mm$=right$(mm$,len(mm$)-1):o$="Message #"+mm$:gosub5100
 5005 o$="From:"+un$+cr$+"Date:"+da$+cr$+"Subject:"+cr$+su$:gosub5100
 5009 o$="{clr}EDITOR"+cr$+cr$+"Start typing."+cr$+"Word Wrap:ON."+cr$+"Type /? for help!"
 5010 o$=o$+cr$:sysa(1)
 5013 o$="{CBM-T}{CBM-T}{CBM-T}{CBM-T}{CBM-T}{CBM-T}{CBM-T}{CBM-T}{CBM-T}{CBM-T}{CBM-T}{CBM-T}{CBM-T}{CBM-T}{CBM-T}{CBM-T}{CBM-T}{CBM-T}{CBM-T}{CBM-T}{CBM-T}":gosub5100:sysa(1):mc=21
 5015 gosub400:ifi$="/s"then5020
 5016 ifi$="/a"then5090
 5017 ifi$="/e"ori$="/l"ori$="/p"ori$="/w"ori$="/d"ori$="/?"then5200
 5018 ifcd=0then5090
 5019 o$=i$:gosub5100:goto5015
 5020 nm=nm+1:o$=cr$+"{wht}Saving..."+cr$:sysa(1):open15,8,15
 5021 ifem=1then5099
 5025 open3,8,3,"m"+str$(nm)+",s,w":fork=qrtoqr+qx:print#3,chr$(peek(k));:nextk:close3
 5030 input#15,e,e$:close15:gosub3100
 5040 o$="{down}Status:"+e$:sysa(1)
 5045 gosub 1800
 5050 goto 5099
 5090 o$=cr$+"Aborted!"+cr$:sysa(1):mc=0
 5099 ww=0:return
 5100 rem * hi ram  *
 5105 o$=o$+cr$
 5110 fork=1tolen(o$)
 5120 pokeqr+qx,asc(mid$(o$,k,1))
 5130 qx=qx+1:ifqx>8192thenqx=8192
 5140 nextk
 5149 return
 5199 return
 5200 rem * editor exp
 5210 ifi$="/e"thengosub5300
 5220 ifi$="/l"thengosub5400
 5230 ifi$="/p"thengosub5500
 5240 ifi$="/w"thengosub5600
 5250 ifi$="/d"thengosub5700
 5260 ifi$="/?"thengosub5800
 5299 goto5015
 5300 rem * edit line
 5302 w1=ww:ww=0
 5305 o$=cr$+"Line # to edit? >":sysa(1):mc=4:poke254,1:gosub400:ifi$=""then5398
 5310 ifval(i$)<1orval(i$)>1000then5398
 5315 o$=cr$+"Searching...":sysa(1):ln=1:q1=0:q2=0:k=qr:z=val(i$)+6
 5320 ifln=zandk<qr+qxthen5350
 5325 ifchr$(peek(k))=cr$thenln=ln+1
 5330 k=k+1:ifk>qr+qxthen5397
 5335 goto5320
 5350 q1=k:el$=""
 5355 z$=chr$(peek(k)):ifz$=cr$then5360
 5356 el$=el$+z$
 5358 k=k+1:ifk>qr+qxthen5397
 5359 goto5355
 5360 q2=k:o$=cr$+cr$+"Original Line:"+cr$+">"+el$+cr$:sysa(1)
 5365 o$=cr$+"Enter New Line:"+cr$+">":sysa(1):mc=21:poke254,1
 5366 gosub400:ifi$=""then5398
 5367 o$=cr$+"Working...":sysa(1)
 5368 df=len(i$)-len(el$)
 5370 ifdf<0thenforw=q2toqr+qx:pokew+df,peek(w):nextw
 5372 ifdf>0thenforw=qr+qxtoq2step-1:pokew+df,peek(w):nextw
 5375 qx=qx+df
 5380 i$=i$+cr$:forw=1tolen(i$):pokew-1+q1,asc(mid$(i$,w,1)+chr$(0)):nextw
 5385 o$=cr$+"Replaced!"+cr$:sysa(1):gosub1800:goto5398
 5397 o$=cr$+"{rvon}NOT{$a0}FOUND.{rvof}"+cr$:sysa(1):gosub1800
 5398 gosub5500
 5399 ww=w1:mc=21:return
 5400 rem * list msg
 5410 o$="{clr}":sysa(1)
 5415 o$="":ln=1
 5420 fork=qrtoqr+qx
 5430 z$=chr$(peek(k))
 5440 o$=o$+z$
 5450 ifz$=cr$andln>6theno$=cr$+str$(ln-6)+":"+cr$+o$:sysa(1):o$="":ln=ln+1
 5455 ifz$=cr$andln<=6thensysa(1):o$="":ln=ln+1
 5460 nextk
 5499 return
 5500 rem * preview
 5510 o$="{clr}":sysa(1)
 5515 o$=""
 5520 fork=qrtoqr+qx
 5530 z$=chr$(peek(k))
 5540 o$=o$+z$
 5550 ifz$=cr$thensysa(1):o$=""
 5560 nextk
 5599 return
 5600 rem * toggle ww
 5610 ifww=0thenww=1:o$="{rvon}WORD{$a0}WRAP:ON{rvof}"+cr$:sysa(1):goto5699
 5620 ifww=1thenww=0:o$="{rvon}WORD WRAP:OFF{rvof}"+cr$:sysa(1):goto5699
 5699 return
 5700 rem * del line
 5702 w1=ww:ww=0
 5705 o$=cr$+"Line # to delete? >":sysa(1):mc=4:poke254,1:gosub400:ifi$=""then5798
 5710 ifval(i$)<1orval(i$)>1000then5798
 5715 o$=cr$+"Searching...":sysa(1):ln=1:q1=0:q2=0:k=qr:z=val(i$)+6
 5720 ifln=zandk<qr+qxthen5750
 5725 ifchr$(peek(k))=cr$thenln=ln+1
 5730 k=k+1:ifk>qr+qxthen5797
 5735 goto5720
 5750 q1=k:el$=""
 5755 z$=chr$(peek(k)):ifz$=cr$then5760
 5756 el$=el$+z$
 5758 k=k+1:ifk>qr+qxthen5797
 5759 goto5755
 5760 q2=k:o$=cr$+cr$+"Delete Line:"+cr$+">"+el$+cr$:sysa(1)
 5765 gosub1850:ifa$<>"y"then5798
 5767 o$=cr$+"Working...":sysa(1)
 5768 df=len(el$)+1
 5770 forw=q2toqr+qx:pokew-df,peek(w):nextw
 5775 qx=qx-df
 5780 rem
 5785 o$=cr$+"Deleted!"+cr$:sysa(1):gosub1800:goto5798
 5797 o$=cr$+"{rvon}NOT{$a0}FOUND.{rvof}"+cr$:sysa(1):gosub1800
 5798 gosub5500
 5799 ww=w1:mc=21:return
 5800 rem * edit help
 5810 o$="/s=save, /e=edit,"+cr$+"/l=list, /p=preview,"+cr$+"/w=wrap, /a=abort,"+cr$
 5820 sysa(1)
 5899 return
 6000 rem * sysop menu
 6001 ifval(sl$)<4then6099
 6005 o$=cr$+"{wht}Sysop:":sysa(1):poke254,1:sysa(3):a$=chr$(peek(780)):o$=a$+cr$:sysa(1)
 6006 ifcd=0then6099
 6010 ifa$="e"ora$="E"thengosub6200:goto6005
 6096 ifa$="x"ora$="X"then6099
 6097 ifa$="?"thengosub6100:goto6005
 6098 o$=cr$+"Press ? for Help!"+cr$:sysa(1):goto6005
 6099 return
 6100 rem * sysop help
 6110 o$=cr$+"SYSOP{$a0}MENU"+cr$+cr$+"{rvon}E{rvof} - Edit a File"+cr$+cr$+"{rvon}X{rvof} - Exit Sysop{$a0}Area"
 6115 o$=o$+cr$
 6120 sysa(1)
 6199 return
 6200 rem * edit file
 6205 o$=cr$+"Enter File Name:"+cr$+">":poke254,1:mc=16:gosub400:ef$=i$:o$=cr$:sysa(1)
 6206 ifcd=0then6299
 6210 o$=cr$+"Drive? (8-11):":mc=2:goub400:ed=val(i$)
 6211 ifcd=0then6299
 6212 ifed<>8anded<>9anded<>10anded<>11then6210
 6220 em=1:gosub5009:ifem=0then6299
 6299 em=0:return

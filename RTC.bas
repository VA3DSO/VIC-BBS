   10 print"{clr}{rvon}    REALTIME CLOCK    {rvof}";chr$(14)
   20 print"Time: ";:gosub100
   30 print"Date: ";:gosub200:print
   31 print"Enter new data?"
   32 geta$:ifa$=""then32
   33 ifa$<>"y"thenend
   35 print
   36 print"Use spaces instead of"
   37 print"colons or slashes."
   38 print"Enter two digits per"
   39 print"number.":print
   40 input"New Time";nt$
   50 input"New Date";nd$
   60 print:print"Setting RTC..."
   70 gosub 300
   99 end
  100 rem * show time *
  110 h=peek(38916)
  120 m=peek(38914)
  130 s=peek(38912)
  140 h$=str$(h)
  145 h$=right$(h$,len(h$)-1)
  150 m$=str$(m)
  155 m$=right$(m$,len(m$)-1)
  160 s$=str$(s)
  165 s$=right$(s$,len(s$)-1)
  190 printh$;":";m$;":";s$
  199 return
  200 rem * show date *
  210 d=peek(38919)
  220 m=peek(38920)
  230 y=peek(38921)
  240 d$=str$(d)
  245 d$=right$(d$,len(d$)-1)
  250 m$=str$(m)
  255 m$=right$(m$,len(m$)-1)
  260 y$=str$(y)
  265 y$=right$(y$,len(y$)-1)
  290 printy$;"/";m$;"/";d$
  299 return
  300 rem * set rtc *
  310 h=val(left$(nt$,2))
  320 m=val(mid$(nt$,4,2))
  330 s=val(right$(nt$,2))
  340 y=val(left$(nd$,2))
  350 n=val(mid$(nd$,4,2))
  360 d=val(right$(nd$,2))
  370 poke38922,0
  380 poke38923,128+7
  390 poke38919,d
  400 poke38920,n
  410 poke38921,y
  420 poke38916,h
  430 poke38914,m
  440 poke38912,s
  450 poke38923,7
  460 poke38922,32
  499 return

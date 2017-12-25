unit Timer;

{ Copyright 1997 by George M. Tzoumas }

{ This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
Use this software AT YOUR OWN RISK. }

interface

var Elapsed : LongInt;

procedure TimerStart;
procedure TimerStop;
procedure WriteElapsedTime;
procedure WriteElapsedSeconds;
procedure WriteElapsedTicks;

implementation

uses Crt;

var
  SysTimer: Longint Absolute $40:$6C;
  TimeStart: Longint;

procedure TimerStart;
begin
  TimeStart := SysTimer;
end;

procedure TimerStop;
begin
  Elapsed := SysTimer - TimeStart;
end;

procedure WriteElapsedTime;
var H, LR: LongInt;
    M, S: Byte;
    hh: Word;
    R: Real;
begin
  R:=Elapsed / 18.2;
  hh:=Trunc((R-Trunc(R))*1000);
  LR:=Trunc(R);
  H:=LR div 3600;
  Dec(LR, H*3600);
  M:=LR div 60;
  Dec(LR, M*60);
  S:=LR;
  Writeln('Elapsed Time : ', H:11,':',M:2,':',S:2,'.',hh:3);
end;

procedure WriteElapsedSeconds;
begin
  Writeln('Seconds : ', Elapsed / 18.2:12:12)
end;

procedure WriteElapsedTicks;
begin
  Writeln('Ticks : ', Elapsed)
end;

end.
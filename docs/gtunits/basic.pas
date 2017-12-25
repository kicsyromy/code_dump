unit Basic;

{ Basic Unit, Version 6.35, (c) 1992 - 1997 by George M. Tzoumas }

{ This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
Use this software AT YOUR OWN RISK. }

{ 31 Aug 1998 }

interface

type

    TPoint = record x, y: Integer end;
    TScanLines = record Top, Bottom: Byte end;
    Str79 = String[79];
    ListArray = array[1..180] of str79;
    CharSet = Set of Char;

    TColorSet = record tf, tb, hf, hb, ff, fb: byte end;

    MenuRec = record
                Pos: TPoint;
                Chos: array[1..30] of Str79;
                Poses: array[1..30] of TPoint;
                Shortcut: CharSet;
                St: Integer; { first item in chos }
                Ed: Integer; { last item in chos }
                Cur: Integer; { selected item }
                Framed: Boolean;
                Colors: TColorSet;
                ExitKeys: CharSet;
                ExitKey: Char;
              end; { Record }

    ListRec = record
                Pos: TPoint;
                Len: Word;
                Framed: Boolean;
                Colors: TColorSet;
                chos: ListArray;
                St, Ed, Cur, Top, MaxVis: Integer;
              end;

    String8 = String[8];

const
     DoubleBox: String8 = 'ÉÍ»ÈÍ¼ºº';
     SingleBox: String8 = 'ÚÄ¿ÀÄÙ³³';
     ThickBox: String8 = 'ÜÜÜßßßÛÛ';
     WindowBox: String8 = 'ÕÍ¸ÀÄÙ³³';

     programmer = '¸š˜šß²Ñß«…Š’žŒ';
     copyright = '¼†–˜—‹ß×œÖ';
     version = '©šŒ–‘';

     csThin   = $00;
     csNormal = $01;
     csHalf   = $02;
     csFull   = $03;

     DefaultSet: TColorSet= (tf:  7; tb:  1; hf: 15; hb:  2; ff: 15; fb:  1);

     BlackWhite: TColorSet= (tf: 15; tb:  0; hf:  0; hb:  7; ff: 15; fb:  0);

var
   VideoSeg: Word;
   CursorType: Byte;
   CursorVis: Boolean;
   CursorTypes: Array [0..3] of TScanLines;
   ColorSet: TColorSet;
   rem: string;

function Attr(Foreground, Background: Byte): Byte;
function Rows: Byte;
function Cols: Byte;
function ScrAddr(col, row: Byte): Word;
function Screen(col, row: Byte): Byte;
function ScrFG(col, row: Byte): Byte;
function ScrBG(col, row: Byte): Byte;
function ScrAttr(col, row: Byte): Byte;
procedure ChangeAttr(x1, y, x2: Integer; Attr: Byte);
procedure Retrace;

function DecodeStr(const s: string): String;
function Checksum(const s: string): Word;
function VerNum(verhi, verlo: byte): String;

procedure WriteXY(x, y: Byte; Message: String);
procedure Color(ForeGround, BackGround: Byte);
procedure Center(row: Byte; Message: Str79);
procedure Sign(row: Byte; Message: Str79);
procedure Frame(x1, y1, x2, y2, Style: Byte);
procedure MakeBox(x1, y1, x2, y2: Byte; Box: String8);

procedure Reboot; inline ($EA/$00/$00/$FF/$FF);  { jump FFFF: 0 }
procedure Snd(Frequency, Milliseconds: Integer);
procedure Beep;

procedure Cls(x1, y1, x2, y2: Byte);
procedure WinOpen(x1, y1, x2, y2, fg, bg, ff, fb: Byte; WinTitle: Str79);
{
** old pascal routines **
procedure GetText(x1, y1, x2, y2: Byte; var Buffer: Pointer);
procedure PutText(x1, y1: Byte; Buffer: Pointer);
}

procedure GetText(x1, y1, x2, y2: Byte; var Buffer);
procedure PutText(x1, y1: Byte; var Buffer);

procedure ReadStr(var st: String; prompt: String; ls, len, fg, bg, ffg, fbg: Byte);
procedure GetString(var str: string; numchar, len, fg, bg: byte);
procedure Message(st: Str79; prompt: Str79; fg, bg, ffg, fbg: Byte; ch:char);
procedure ShowCursor;
procedure HideCursor;
procedure SetCursorType(csType: Byte);
procedure DefaultList(var l: ListRec);
function MakeList(var l: ListRec): Integer;

implementation

uses Crt, Dos, Mouse, StrHan;

function Attr(Foreground, Background: Byte): Byte; assembler;
asm
   mov al, Background
   shl al, 4
   or al, Foreground
end;

function Rows: Byte;
begin
  Rows:=Byte(WindMax)-Byte(WindMin)+1;
end;

function Cols: Byte;
begin
  Cols:=Lo(WindMax)-Lo(WindMin)+1;
end;

function ScrAddr(col, row: Byte): Word; assembler;
asm
   xor ax, ax                   { offset:=(row-1)*160+((col-1)*2); }
   xor bx, bx
   xor cx, cx
   dec row
   mov cl, row
   mov ax, cx
   mov bl, 80
   mul bl
   dec col
   mov bl, col
   add ax, bx
   shl ax, 1
end;


function Screen(col, row: Byte): Byte;
begin
     screen:=Mem[VideoSeg: ScrAddr(col, row)];
end;

function ScrFG(col, row: Byte): Byte;
var offset: Word;
    a, b: Byte;
begin
     a:=mem[VideoSeg: ScrAddr(col, row) + 1];
     b:=a mod 16;
     scrfg:=a-b shl 4;
end;

function ScrBG(col, row: Byte): Byte;
begin
     scrBG:=Mem[videoseg: ScrAddr(col, row) + 1] mod 16
end;

function ScrAttr(col, row: Byte): Byte;
begin
     scrAttr:=mem[videoseg: ScrAddr(col, row)+1];
end;

function DecodeStr(const s: string): string;
var i, j : byte;
    t : string;
begin
  t:='';
  for i := 1 to length(s) do t:=t+chr(not(ord(s[i])));
  decodestr:=t;
end;

procedure ChangeAttr(x1, y, x2: Integer; Attr: Byte); assembler;
asm
	mov     ax, y
	dec     ax
	add     al, windmin.1.byte
	adc     ah, 0
	mov     cx, 80
	mul     cx
	add     ax, x1
	dec     ax
	add     al, windmin.0.byte
	adc     ah, 0
	shl     ax, 1
	mov     di, ax
	mov     es, videoseg
	mov     cx, x2
	sub     cx, x1
	add     cx, 2
	cld
	mov     al, attr
@@1:    inc     di
	stosb
	loop    @@1
end;

procedure Retrace; assembler;
asm
        cmp lastmode, 7
        je @exit
        mov dx,3dah
@l1:    in al,dx
        test al,8
        jnz @l1
@l2:    in al,dx
        test al,8
        jz @l2
@exit:
end;

function Checksum(const s: string): word;
var i: byte;
    c: word;
begin
  c:=0;
  for i:=1 to length(s) do c:=c+ord(s[i]);
  checksum:=c;
end;

function VerNum(verhi, verlo: byte): string;
var ver: string;
begin
  ver:=IntToStr(verhi);
  ver:=ver+'.'+IntToStr(verlo);
  vernum:=ver;
end;

procedure WriteXY(x, y: Byte; message: String);
begin
     GotoXY(x, y);
     Write(message);
end;

procedure Color(Foreground, Background: Byte);
begin
     TextAttr:=Attr(Foreground, Background)
end;

procedure Center(row: Byte;message: Str79);
begin
     WriteXY((Cols - Length(message)) shr 1, row, message)
end;

procedure Sign(row: Byte;message: Str79);
begin
     Gotoxy(1, row);
     ClrEol;
     Center(row, message);
end;

procedure Frame(x1, y1, x2, y2, style: Byte);
var ul, ur, dl, dr, uh, dh, vr: char;
    j: Integer;
begin
     if style=0 then
     begin
          ul:=#220;ur:=#220;dl:=#223;dr:=#223;uh:=#220;dh:=#223;vr:=#219;
     end;
     if style=1 then
     begin
          ul:=#218;ur:=#191;dl:=#192;dr:=#217;uh:=#196;dh:=#196;vr:=#179;
     end;
     if style=2 then
     begin
          ul:=#201;ur:=#187;dl:=#200;dr:=#188;uh:=#205;dh:=#205;vr:=#186;
     end;
     if style > 2 then
     begin
          ul:=chr(style);ur:=chr(style);dl:=chr(style);dr:=chr(style);uh:=chr(style);dh:=chr(style);vr:=chr(style);
     end;
     writexy(x1, y1, ul);
     for j:=x1+1  to x2-1 do writexy(j, y1, uh);
     writexy(x2, y1, ur);
     for j:=y1+1 to y2-1 do
     begin
          writexy(x1, j, vr);
          writexy(x2, j, vr);
     end;
     writexy(x1, y2, dl);
     for j:=x1+1  to x2-1 do writexy(j, y2, dh);
     writexy(x2, y2, dr);
end;

procedure MakeBox(x1, y1, x2, y2: Byte;Box: String8);
var j: Integer;
begin
     writexy(x1, y1, box[1]);
     for j:=x1+1  to x2-1 do writexy(j, y1,box[2]);
     writexy(x2, y1, box[3]);
     for j:=y1+1 to y2-1 do
     begin
          writexy(x1, j, box[7]);
          writexy(x2, j, box[8]);
     end;
     writexy(x1, y2, box[4]);
     for j:=x1+1  to x2-1 do writexy(j, y2, box[5]);
     writexy(x2, y2, box[6]);
end;

procedure Beep;
begin
  Snd(800, 250);
end;

procedure Snd(Frequency, Milliseconds: Integer);
begin
     Sound(Frequency);
     Delay(Milliseconds);
     Nosound;
end;

procedure Cls(x1, y1, x2, y2: Byte);
var b: pointer;
begin
     getmem(b, (x2-x1+1)*(y2-y1+1) shl 1);
     FillChar(b^, (x2-x1+1)*(y2-y1+1) shl 1+2, Chr(0));
     mem[Seg(b^): Ofs(b^)]:=(x2-x1+1);
     mem[Seg(b^): Ofs(b^)+1]:=(y2-y1+1);
     PutText(x1, y1, b);
     freemem(b, (x2-x1+1)*(y2-y1+1) shl 1);
end;

procedure WinOpen(x1, y1, x2, y2, fg, bg, ff, fb: Byte; WinTitle: Str79);
var i: integer;
begin
     window(1, 1, 80, 25);
     color(ff, fb);
     MakeBox(x1, y1, x2, y2, WindowBox);
     if length(wintitle)>0 then
          writexy(x1+(x2-x1+1-length(wintitle)) shr 1, y1, ' '+wintitle+' ');
     textbackground(0);
     ChangeAttr(x1+2,y2+1,x2+1, 7);
     for i:=y1+1 to y2 do changeattr(x2+1,i,x2+1,7);
     window(x1+1, y1+1, x2-1, y2-1);
     Color(fg, bg);
     clrscr;
end;


{procedure GetText(x1, y1, x2, y2: Byte; var Buffer: Pointer);
var S, O: Word;
    h, i, j: Word;
begin
     S:=Seg(Buffer^);
     O:=Ofs(Buffer^);
     mem[S: O+1]:=y2-y1+1;
     mem[S: O]:=x2-x1+1;
     h:=2;
     for i:=y1 to y2 do for j:=x1 to x2 do
     begin
          memw[S: O+h]:=memw[videoseg: ScrAddr(j, i)];
          h:=h+2;
     end;
end;

procedure PutText(x1, y1: Byte; Buffer: Pointer);
var S, O: Word;
    h, i, j: Word;
begin
     S:=Seg(Buffer^);
     O:=Ofs(Buffer^);
     h:=2;
     for i:=1 to mem[S: O+1] do for j:=1 to mem[S: O] do
     begin
          memw[videoseg: scraddr(x1-1+j, y1-1+i)]:=memw[S: O+h];
          h:=h+2;
     end;
end;}

procedure GetText(x1, y1, x2, y2: Byte; var Buffer);
var o, w, h: Word;
begin
  o := ScrAddr(x1, y1);
  w := x2 - x1 + 1;
  h := y2 - y1 + 1;
asm
      push ds
      mov ds, VideoSeg
      mov si, o
      les di, Buffer
      cld
      xor ah, ah
      mov ax, w
      stosb
      mov ax, h
      stosb
      xor ch, ch
      mov cl, al
      mov bx, 80
      sub bx, w
      shl bx, 1
@@1:  push cx
      mov cx, w
      rep movsw
      add si, bx
      pop cx
      loop @@1
      pop ds
end;
end;

procedure PutText(x1, y1: Byte; var Buffer);
var o, w, h: Word;
begin
  o := ScrAddr(x1, y1);
asm
      push ds
      mov es, VideoSeg
      mov di, o
      lds si, Buffer
      cld
      xor ah, ah
      lodsb
      mov w, ax
      lodsb
      mov cx, ax
      xor ch, ch
      mov bx, 80
      sub bx, w
      shl bx, 1
@@1:  push cx
      mov cx, w
      rep movsw
      add di, bx
      pop cx
      loop @@1
      pop ds
end;
end;

procedure ReadStr(var st: String; prompt: String; ls, len, fg, bg, ffg, fbg: Byte);
var p: pointer;
begin
     getmem(p, (len+1) shl 2 * 3 );
     gettext(40-len shr 1, 11, 40+len shr 1+2, 14, p^);
     winopen(40-len shr 1, 11, 40+len shr 1, 13, fg, bg, ffg, fbg, prompt);
     getstring(st, ls, len-2, fg, bg);
     window(1, 1, 80, 25);
     puttext(40-len shr 1, 11, p^);
     freemem(p, (len+1) shl 2 * 3);
end;

procedure GetString(var str: string; numchar, len, fg, bg: byte);
var s: string;
    sx, sy: byte;
    xpos, lpos: byte;
    ins, cursor: boolean;
    ct: byte;
    cs: string;
    ch, cf:char;

procedure viewstr;
var i:byte;
begin
     gotoxy(sx,sy);
     write(copy(s, lpos-xpos+1, len));
     i:=len - (wherex-sx);
     if i>0 then write(' ': i);
     gotoxy(sx+xpos-1, sy);
end;

begin
     cursor:=not CursorVis;
     ct:=cursortype;
     ShowCursor;
     s:=str;
     sx:=wherex;
     lpos:=length(s)+1;
     xpos:=len;
     if xpos>lpos then xpos:=lpos;
     sy:=wherey;
     ins:=true;
     textcolor(fg);
     textbackground(bg);
     write(' ':len);
     gotoxy(sx,sy);
     repeat;
            viewstr;
            cs:='';
            ch:=readkey;
            cs:=ch;
            case ch of
            #32..#255: if (length(s)<numchar) or ((lpos<numchar+1) and (not ins)) then
            begin
                 if not ins then delete(s,lpos,1);
                 insert(cs, s, lpos);
                 inc(lpos);
                 if xpos<len then inc(xpos);
            end;
            #8: if lpos>1 then begin
                     dec(lpos);
                     delete(s, lpos, 1);
                     if (xpos>1) and (xpos>lpos) then dec(xpos);
                end;
            #25: begin
                   s:='';
                   lpos:=1;
                   xpos:=1;
                 end;
            #26: begin
                   s:=str;
                   lpos:=length(s)+1;
                   xpos:=len;
                   if xpos>lpos then xpos:=lpos;
                 end;
            #0: begin
                     cf:=readkey;
                     case cf of
                     #75: if lpos>1 then begin
                            dec(lpos);
                            if (xpos>1) then dec(xpos);
                          end;
                     #77: begin
                            if lpos<=length(s) then inc(lpos);
                            if (xpos<len) and (xpos<lpos) then inc(xpos);
                          end;
                     #82: begin
                            ins:=not ins;
                            if not ins then SetCursorType(csFull)
                              else SetCursorType(csNormal);
                          end;
                     #83: delete(s, lpos, 1);
                     #71: begin
                            lpos:=1;
                            xpos:=1;
                          end;
                     #79: begin
                            lpos:=length(s)+1;
                            xpos:=len;
                            if xpos>lpos then xpos:=lpos;
                          end;
                     end; {case #0}
                end;
            end;
     until (ch=#13) or (ch=#27);
     textattr:=7;
     if ch=#13 then str:=s;
     if cursor then HideCursor;
     SetCursorType(ct);
end;

procedure Message(st: Str79; prompt: Str79; fg, bg, ffg, fbg: Byte; ch:char);
var p: pointer;
    len: byte;
begin
     len:=length(st)+3;
     if len < length(prompt) then len:=length(prompt)+3;
     getmem(p, (len+1) shl 2 * 3 );
     gettext(40-len shr 1, 11, 40+len shr 1+2, 14, p^);
     winopen(40-len shr 1, 11, 40+len shr 1, 13, fg, bg, ffg, fbg, prompt);
     center(1,st);
     repeat
     until readkey=ch;
     window(1, 1, 80, 25);
     puttext(40-len shr 1, 11, p^);
     freemem(p, (len+1) shl 2 * 3);
end;

procedure ShowCursor;
var Top, Bot: Byte;
begin
  Top := CursorTypes[CursorType].Top;
  Bot := CursorTypes[CursorType].Bottom;
  asm
       mov ah, 01h
       mov ch, Top
       mov cl, Bot
       int 10h
       mov CursorVis, 1
  end;
end;

procedure HideCursor; assembler;
asm
     mov ah, 01h
     mov ch, 20h
     mov cl, 00h
     int 10h
     mov CursorVis, 0
end;

procedure SetCursorType(csType: Byte);
begin
  if csType > 3 then csType := csNormal;
  CursorType := csType;
  if CursorVis then ShowCursor;
end;

procedure DefaultList(var l: ListRec);
begin
  l.pos.y:=7;
  l.len:=40;
  l.pos.x:=(Cols-l.len) shr 1;
  l.framed:=true;
  l.colors:=colorset;
  l.st:=1;
  l.cur:=1;
  l.top:=1;
  l.maxvis:=12;
end;

function MakeList(var l: ListRec): Integer;
var p, top : Integer;
    ch, cf: Char;

  procedure HiOff;
  begin
    Color(l.colors.tf,l.colors.tb);
  end;

  procedure HiOn;
  begin
    Color(l.colors.hf,l.colors.hb);
  end;

  procedure view(i: Integer);
  begin
    if (i<=l.ed) or (i>=l.st) then write(l.chos[i]);
    clreol;
  end;

  procedure refresh;
  var j,i: integer;
  begin
    HiOff;
    j:=1;
    for i:=top to top+l.MaxVis-1 do
    begin
      gotoxy(1, j);
      view(i);
      inc(j);
    end;
    hion;
    gotoxy(1, p-top+1);
    view(p);
  end;

  procedure godown;
  begin
    if p=l.ed then exit;
    hioff;
    gotoxy(1, p-top+1);
    view(p);
    if p+1>l.ed then dec(p);
    inc(p);
    if p>top+l.MaxVis-1 then
    begin
      inc(top);
      gotoxy(1,1);
      delline;
    end;
    hion;
    gotoxy(1, p-top+1);
    view(p);
  end;

  procedure goup;
  begin
    if p=l.st then exit;
    hioff;
    gotoxy(1, p-top+1);
    view(p);
    if p-1<l.st then inc(p);
    dec(p);
    if p<top then
    begin
      dec(top);
      insline;
    end;
    gotoxy(1, p-top+1);
    hion;
    view(p);
  end;

  procedure pagedown;
  var i: integer;
  begin
    for i:=1 to l.MaxVis-1 do godown;
  end;

  procedure pageup;
  var i: integer;
  begin
    for i:=1 to l.MaxVis-1 do goup;
  end;

  procedure gohome;
  var i:integer;
  begin
    if p=l.st then exit;
    p:=l.st;
    top:=p;
    refresh;
  end;

  procedure goend;
  var i: integer;
  begin
    if p=l.ed then exit;
    p:=l.ed;
    top:=l.ed-l.MaxVis+1;
    refresh;
  end;

begin
  Window(1,1, 80, 25);
  p:=l.cur;
  top:=l.top;
  color(l.colors.ff,l.colors.fb);
  if l.framed then
  begin
    makebox(l.pos.x, l.pos.y, l.pos.x+l.len, l.pos.y+l.MaxVis+1, WindowBox);
    Window(l.pos.x+1, l.pos.y+1, l.pos.x+l.len-1, l.pos.y+l.MaxVis);
  end else Window(l.pos.x, l.pos.y, l.pos.x+l.len, l.pos.y+l.MaxVis-1);
  refresh;
  repeat
  ch:=readkey;
  case ch of
  #0:begin
       cf:=readkey;
       case cf of
       #80:godown;
       #72:goup;
       #73:pageup;
       #81:pagedown;
       #71:gohome;
       #79:goend;
       end;                      { case arrow }
     end;                        { case #0 }
  end;                           { case general }
  until (ch=#27) or (ch=#13);
  if ch=#13 then
  begin
    l.cur:=p;
    l.top:=top;
    MakeList:=p;
  end else MakeList:=-1;
end;


procedure Init;
  procedure InitMono;
  begin
    VideoSeg := SegB000;
    CursorTypes[0].top:=13;
    CursorTypes[0].Bottom:=13;
    CursorTypes[1].top:=12;
    CursorTypes[1].Bottom:=13;
    CursorTypes[2].top:=8;
    CursorTypes[2].Bottom:=13;
    CursorTypes[3].top:=0;
    CursorTypes[3].Bottom:=13;
    ColorSet:=BlackWhite;
  end;
  procedure InitColor;
  begin
    VideoSeg := SegB800;
    CursorTypes[0].top:=7;
    CursorTypes[0].Bottom:=7;
    CursorTypes[1].top:=6;
    CursorTypes[1].Bottom:=7;
    CursorTypes[2].top:=4;
    CursorTypes[2].Bottom:=7;
    CursorTypes[3].top:=0;
    CursorTypes[3].Bottom:=7;
    ColorSet:=DefaultSet;
  end;
begin
  SetCursorType(csNormal);
  if LastMode=Mono then InitMono else InitColor;
  CursorVis:=True;
  SetCursorType(csNormal);
end;

begin
  rem:='* Portions Copyright 1992 - 1997 by George M. Tzoumas *';
  if CheckSum(rem)<>4393 then Halt($FFFF);
  Init;
end.
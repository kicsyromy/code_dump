unit Crt2;

{ Copyright 1997 by George M. Tzoumas }

{ 11 Oct 1997 }

{ This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
Use this software AT YOUR OWN RISK. }

interface

var
  VideoSeg : Word;
  WhereX, WhereY : Byte;
  TextAttr : Byte;
  CursorTrack: Boolean;

function CharOffs(x, y: Byte): Word;
procedure TrackCursor;
procedure CameraCursor;
procedure WriteChar(x, y: Byte; c: Char);
procedure WriteString(x, y: Byte; s: String);
procedure WritelnStr(s: String);
procedure WriteStr(s: String);
procedure ClrScr;
procedure DelLine(l: Byte);
procedure InsLine(l: Byte);
procedure GotoXY(x, y: Byte);
procedure Retrace;


implementation

function CharOffs(x, y: Byte): Word; assembler;
asm
        dec x
        dec y
        mov al, y
        mov bl, 160
        mul bl
        mov bl, x
        xor bh, bh
        shl bx, 1
        add ax, bx
end;

procedure TrackCursor; assembler;
asm
        mov dl, WhereX
        mov dh, WhereY
        dec dl
        dec dh
        xor bh, bh
        mov ah, $02
        int $10
end;

procedure CameraCursor; assembler;
asm
        mov ah, $03
        xor bh, bh
        int $10
        inc dl
        inc dh
        mov WhereX, dl
        mov WhereY, dh
end;

procedure WriteChar(x, y: Byte; c: Char);
begin
  MemW[VideoSeg: CharOffs(x, y)] := TextAttr shl 8 or Byte(C);
end;

procedure WriteString(x, y: Byte; s: String);
var i, j: Byte;
    o: Word;
begin
  o := CharOffs(x, y);
  j := 0;
  while j < Byte(s[0]) do
  begin
    Inc(j);
    MemW[VideoSeg: o] := TextAttr shl 8 or Byte(s[j]);
    Inc(o, 2);
  end;
end;

procedure WritelnStr(s: String);
begin
  WriteStr(s);
  WhereX := 1;
  if WhereY < 25 then Inc(WhereY) else DelLine(1);
  if CursorTrack then TrackCursor;
end;

procedure WriteStr(s: String);
begin
  WriteString(WhereX, WhereY, s);
  Inc(WhereX, Byte(s[0]));
  if WhereX > 80 then
  begin
    Dec(WhereX, 80);
    if WhereY < 25 then Inc(WhereY) else DelLine(1);
  end;
  if CursorTrack then TrackCursor;
end;

procedure DelLine(l: Byte);
var o1, o2, c: Word;
begin
  o1 := CharOffs(1, l);
  o2 := o1 + 160;
  c := (4000 - o2) shr 1;
  asm
        mov ax, VideoSeg
        mov es, ax
        mov si, o2
        mov di, o1
        cld
        mov cx, c
        push ds
        mov ds, ax
        rep movsw
        pop ds
        mov ah, TextAttr
        mov al, $20
        mov cx, 80
        rep stosw
  end;
end;

procedure InsLine(l: Byte);
var o1, o2, c: Word;
begin
  o1 := CharOffs(80, 24);
  o2 := o1 + 160;
  c := (25 - l) * 80;
  asm
        mov ax, VideoSeg
        mov es, ax
        mov si, o1
        mov di, o2
        std
        mov cx, c
        push ds
        mov ds, ax
        rep movsw
        pop ds
        mov ah, TextAttr
        mov al, $20
        mov cx, 80
        rep stosw
  end;
end;

procedure ClrScr; assembler;
asm
        mov WhereX, 1
        mov WhereY, 1
        mov ah, TextAttr
        mov al, $20
        mov cx, 2000
        mov es, VideoSeg
        xor di, di
        cld
        rep stosw
        mov al, CursorTrack
        cmp al, True
        jnz @end
        call TrackCursor
@end:
end;

procedure GotoXY(x, y: Byte);
begin
  if (X = 0) or (X > 80) or (Y = 0) or (Y > 25) then Exit;
  WhereX := x;
  WhereY := y;
  if CursorTrack then TrackCursor;
end;

procedure Retrace; assembler;
asm
        cmp videoseg, 0
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

begin
  WhereX := Mem[$40:$50] + 1;
  WhereY := Mem[$40:$50+1] + 1;
  TextAttr := 7;
  if Mem[$40:$49] = 7 then VideoSeg := 0 else VideoSeg := $B800;
  CursorTrack := False
end.
unit Fade;

{ Copyright 1997-1998 by George M. Tzoumas }

{ This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
Use this software AT YOUR OWN RISK. }

{ 21 Nov 1998 }

interface

procedure GrabPal;
procedure FadeIn;
procedure FadeOut;
procedure BlackOut;
procedure DefaultPal;

implementation

const FadeSpeed = 256;

type
  PaletteType = array[0..255,1..3] of Byte;

var p, Black: PaletteType;

procedure Retrace; assembler;
asm
        mov   dx, $3DA
@l1:    in    al, dx
        test  al, 8
        jnz   @l1
@l2:    in    al, dx
        test  al, 8
        jz    @l2
end;

procedure GetPalette(Color: Byte; var Red, Green, Blue: Byte); assembler;
asm
   cld
   mov dx, $3C7
   mov al, Color
   out dx, al
   add dx, 2
   les di, Red
   insb
   les di, Green
   insb
   les di, Blue
   insb
end;

procedure SetPalette(Color, Red, Green, Blue: Byte); assembler;
asm
   mov dx, $3C8
   mov al, Color
   out dx, al
   inc dx
   mov al, Red
   out dx, al
   mov al, Green
   out dx, al
   mov al, Blue
   out dx, al
end;

procedure GetAllPalette(var Pal: PaletteType); assembler;
asm
     cld
     les di, Pal
     mov cx, 256
@@1:
     mov dx, $3C7
     mov al, cl
     dec al
     not al
     out dx, al
     add dx, 2
     insb
     insb
     insb
loop @@1
end;

procedure SetAllPalette(Pal: PaletteType); assembler;
asm
     push ds
     cld
     lds si, Pal
     mov cx, 256
@@1:
     mov dx, $3C8
     mov al, cl
     dec al
     not al
     out dx, al
     inc dx
     outsb
     outsb
     outsb
loop @@1
     pop ds
end;

procedure FadeTo(Pal: PaletteType); { linear fade }
type RealPal = array[0..255,1..3] of Longint;
var
  cur, dif: RealPal;
  src: PaletteType;
  i, j: Word;
begin
  GetAllPalette(src);
  for i := 0 to 255 do
  begin
    cur[i,1] := Longint(src[i,1]) shl 14 ; cur[i,2] := Longint(src[i,2]) shl 14; cur[i,3] := Longint(src[i,3]) shl 14;
    dif[i,1] := (Longint(Pal[i,1])-src[i,1]) * FadeSpeed;
    dif[i,2] := (Longint(Pal[i,2])-src[i,2]) * FadeSpeed;
    dif[i,3] := (Longint(Pal[i,3])-src[i,3]) * FadeSpeed;
  end;
  for j := 1 to 16384 div FadeSpeed do
  begin
    Retrace;
    for i := 0 to 255 do
    begin
      SetPalette(i, cur[i,1] shr 14, cur[i,2] shr 14, cur[i,3] shr 14);
      cur[i,1] := cur[i,1] + dif[i,1];
      cur[i,2] := cur[i,2] + dif[i,2];
      cur[i,3] := cur[i,3] + dif[i,3];
    end;
  end;
  SetAllPalette(pal);
end;

procedure GrabPal;
begin
  GetAllPalette(p)
end;

procedure FadeIn;
begin
  FadeTo(p)
end;

procedure FadeOut;
begin
  FadeTo(Black)
end;

procedure BlackOut;
begin
  SetAllPalette(Black)
end;

procedure DefaultPal;
begin
  SetAllPalette(p)
end;

begin
  FillChar(Black, SizeOf(Black), 0);
end.
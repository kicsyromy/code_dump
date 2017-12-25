{ GEORGE TZOUMAS

     VGA.PAS
}

{ This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
Use this software AT YOUR OWN RISK. }

{ VGA 320x200x256 Support Unit, Version 3.2,
   Copyright (C) 1997,98 by George M. Tzoumas  }

{$R-,S-,O-}
unit VGA;

interface

const
  MaxPages = 3;
  GetMaxColor = 255;
  GetMaxX = 319;
  GetMaxY = 199;

type
  PVGATabl = ^TVGATabl;
  TVGATabl = array[0..199,0..319] of Byte;

  PVGABuf = ^TVGABuf;
  TVGABuf = array[0..63999] of Byte;

  PaletteType = array[0..255,1..3] of Byte;

  TPoint = record
     X, Y : Integer;
   end;

var
  Pages: array[0..MaxPages] of PVGATabl;  { array of pages, visible is only page 0 }
  PagesBuf: array[0..MaxPages] of PVGABuf absolute Pages; { a different approach ... }
  YTable: array[0..199] of Word; { contains offset for start of each line }
  NPages: Byte;     { number of pages installed }
  Page: Pointer;    { pointer to current page }
  PageSeg: Word;    { segment of current page (offset is always 0) }
  PageNo: Byte;     { current page number }
  TransColor: Byte; { Transparent Color }

  CurColor: Byte;           { Drawing Color }
  CP: TPoint;               { Current Point (useless ?) }
  OldPalette: PaletteType;  { Initial Palette }


function IsVGA : Boolean; { True if VGA exists }
procedure VGAMode;        { Turns into mode $13, 320x200x256 }
procedure TxtMode;        { Turns into mode $03, Text 80x25  }
procedure Retrace;        { Waits for a vertical retrace     }
function PixelOffs(X, Y: Integer): Word; { Returns the offset of the pixel at x,y }
function InBounds(X, Y: Integer): Boolean; { True if (x,y) in (0,0)-(319,199) }

procedure MoveW(var Source, Dest; Count: Word); { like System's Move, uses words }
procedure MoveD(var Source, Dest; Count: Word); { like System's Move, uses dwords }
procedure FillCharW(var X; Count: Word; value: Word); { like FillChar, uses words }
procedure FillCharD(var X; Count: Word; value: Longint); { like FillChar, uses dwords }

procedure AddPage; { adds a page to our system }
procedure SubPage; { subtracts a page from our system }
procedure AddPages(n: Byte); { adds n pages }
procedure SubPages(n: Byte); { removes n (last) pages }
procedure InstallPage(p: Pointer); { adds a page pointing to p }
procedure UnInstallPage; { removes last page added by InstallPage }
procedure SetPagePtr(p: Pointer); { changes page[0] (SegA000) to p }
procedure SetPage(n: Byte);       { select new page }
procedure RealPage; { setpage(0) }
procedure PageCopy(s, d: Byte); { copys page[s] to page[d] }
procedure Flip(n: Byte); { flip from page ... }
procedure Flop(n: Byte); { flop to page ... }
procedure SaveScreen(FileName: String; SavePal: Boolean); { format: 768 + 64000 = }
procedure LoadScreen(FileName: String; LoadPal: Boolean); { palette + image data }

procedure ClearPage(n: Byte); { clear page n }
procedure Cls; { clear current page }

procedure GetPalette(Color: Byte; var Red, Green, Blue: Byte); { gets the RGB value for color Color }
procedure SetPalette(Color, Red, Green, Blue: Byte);
procedure GetAllPalette(var Pal: PaletteType);
procedure SetAllPalette(Pal: PaletteType);
procedure SetPaletteX(Color: Byte; Pal: PaletteType);
procedure FadeTo(Pal: PaletteType);

procedure PutPixel(X, Y: Integer; Color: Byte);
function GetPixel(X, Y: Integer): Byte;
procedure HLine(X1, X2, Y: Integer);
procedure VLine(X, Y1, Y2: Integer);
procedure HDLine(X, Y: Integer; Len: Word);
procedure VDLine(X, Y: Integer; Len: Word);
procedure Line(X1, Y1, X2, Y2: Integer); { from PCGPE }
procedure Rectangle(X1, Y1, X2, Y2: Integer);
procedure DRectangle(X1, Y1, Width, Height: Integer);
procedure DrawPoly(NumPoints: Word; var PolyPoints); { like Pascal's one }
procedure DrawTriangle(X1, Y1, X2, Y2, X3, Y3: Integer);
procedure DrawSquare(X1, Y1, X2, Y2, X3, Y3, X4, Y4: Integer);

function ImageSize(X1, Y1, X2, Y2: Integer): Word;
procedure GetImage(X1, Y1, X2, Y2: Integer; var BitMap);
procedure PutImage(X, Y: Integer; var BitMap);
procedure TransPutImage(X, Y: Integer; var BitMap);

implementation

uses TVMemory;

function IsVGA : Boolean; assembler;
asm
        mov  ax, $1A00
        int  $10
        cmp  al, $1A
        je   @ok
        mov  ax, False
        jmp  @done
@ok:
        mov  ax, True
@done:
end;

procedure VGAMode;
begin
  asm
      mov  ax, $0013
      int  $10
  end;
  GetAllPalette(OldPalette);
end;

procedure TxtMode; assembler;
asm
   mov  ax, $0003
   int  $10
end;

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

function PixelOffs(X, Y: Integer): Word; assembler;
asm
   mov bx, Y
   add bx, bx
   mov ax, word ptr YTable[bx]
   add ax, X
end;

function InBounds(X, Y: Integer): Boolean; assembler;
asm
   cmp X, 0
   jl @No
   cmp X, 319
   jg @No
   cmp Y, 0
   jl @No
   cmp Y, 199
   jg @No
   mov al, True
   jmp @Ok
@No:
   mov al, False
@Ok:
end;

procedure MoveW(var Source, Dest; Count: Word); assembler;
asm
   push ds
   les di, Dest
   lds si, Source
   mov cx, Count
   cld
   rep movsw
   pop ds
end;

procedure MoveD(var Source, Dest; Count: Word); assembler;
asm
   push ds
   les di, Dest
   lds si, Source
   mov cx, Count
   cld
   rep; db $66; movsw { rep stosd }
   pop ds
end;


procedure FillCharW(var X; Count: Word; value: Word); assembler;
asm
   push es
   les di, X
   mov cx, Count
   mov ax, value
   cld
   rep stosw
   pop es
end;

procedure FillCharD(var X; Count: Word; value: Longint); assembler;
asm
   push es
   les di, X
   mov cx, Count
   db $66
   mov ax, word ptr value
   cld
   rep; db $66; stosw { rep stosd }
   pop es
end;

procedure AddPage;
begin
  Inc(NPages);
  Pages[NPages] := MemAllocSeg(64000);
  ClearPage(NPages);
end;

procedure SubPage;
begin
  FreeMem(Pages[NPages], 64000);
  Dec(NPages);
end;

procedure AddPages(n: Byte);
var i: Byte;
begin
  for i := 1 to n do AddPage;
end;

procedure SubPages(n: Byte);
var i: Byte;
begin
  for i := 1 to n do SubPage;
end;

procedure InstallPage(p: Pointer);
begin
  Inc(NPages);
  Pages[NPages] := p;
end;

procedure UnInstallPage;
begin
  Dec(NPages);
end;

procedure SetPagePtr(p: Pointer);
begin
  PageNo := MaxPages;
  Pages[MaxPages] := p;
  Page := p;
  PageSeg := Seg(Page^);
end;

procedure SetPage(n: Byte);
begin
  PageNo := n;
  Page := Pages[n];
  PageSeg := Seg(Page^);
end;

procedure RealPage;
begin
  SetPage(0);
end;

procedure PageCopy(s, d: Byte);
begin
  MoveD(Pages[s]^, Pages[d]^, 16000);
end;

procedure Flip(n: Byte); { flip from }
begin
  MoveD(Pages[n]^, Page^, 16000);
end;

procedure Flop(n: Byte); { flop to }
begin
  MoveD(Page^, Pages[n]^, 16000);
end;

procedure SaveScreen(FileName: String; SavePal: Boolean);
var
  F: File;
  P: PaletteType;
begin
  Assign(F, FileName);
  Rewrite(F, 1);
  if SavePal then
  begin
    GetAllPalette(P);
    BlockWrite(F, P, 768);
  end;
  BlockWrite(F, Page^, 64000);
  Close(F);
end;

procedure LoadScreen(FileName: String; LoadPal: Boolean);
var
  F: File;
  P: PaletteType;
begin
  Assign(F, FileName);
  Reset(F, 1);
  if LoadPal then
  begin
    BlockRead(F, P, 768);
    SetAllPalette(P);
  end;
  BlockRead(F, Page^, 64000);
  Close(F);
end;

procedure ClearPage(n: Byte);
begin
  FillCharD(Pages[n]^, 16000, 0);
end;

procedure Cls;
begin
  FillCharD(Page^, 16000, 0);
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
     not al        { cool ! }
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

procedure SetPaletteX(Color: Byte; Pal: PaletteType);
begin
  SetPalette(Color, Pal[Color,1], Pal[Color,2], Pal[Color,3]);
end;

procedure FadeTo(Pal: PaletteType);
var i, r, g, b, rr, gg, bb: Byte;
    c: Set of Byte;
begin
  c := [];
  repeat
    for i := 0 to 255 do if i in c then Continue else
    begin
      GetPalette(i, r, g, b);
      rr := r;
      gg := g;
      bb := b;
      if (Pal[i, 1] = r) and (Pal[i, 2] = g) and (Pal[i, 3] = b) then
        Include(c, i);
      if (Pal[i, 1] < r) then Dec(rr);
      if (Pal[i, 1] > r) then Inc(rr);
      if (Pal[i, 2] < g) then Dec(gg);
      if (Pal[i, 2] > g) then Inc(gg);
      if (Pal[i, 3] < b) then Dec(bb);
      if (Pal[i, 3] > b) then Inc(bb);
      SetPalette(i, rr, gg, bb);
    end;
    Retrace;
  until c = [0..255];
end;

procedure PutPixel(X, Y: Integer; Color: Byte); assembler;
asm
    cmp X, 0    { remove clipping for faster performance }
    jl @ok
    cmp X, 319
    jg @ok
    cmp Y, 0
    jl @ok
    cmp Y, 199
    jg @ok
    mov es, PageSeg
    mov bx, Y
    add bx, bx
    mov bx, word ptr YTable[bx]
    add bx, X
    mov al, Color
    mov byte ptr es:[bx], al
@ok:
end;

function GetPixel(X, Y: Integer): Byte; assembler;
asm
    cmp X, 0   { remove clipping for faster performance }
    jl @ok
    cmp X, 319
    jg @ok
    cmp Y, 0
    jl @ok
    cmp Y, 199
    jg @ok
    xor al, al
    mov es, PageSeg
    mov bx, Y
    add bx, bx
    mov bx, word ptr YTable[bx]
    add bx, X
    mov al, byte ptr es:[bx]
@ok:
end;

procedure HLine(X1, X2, Y: Integer); assembler;
asm
{   mov ax, y
   cmp ax, 0
   jl @bye
   cmp ax, 199
   jg @bye
   mov ax, x1
   cmp ax, x2
   jl @ok
   push x1
   push x2
   pop x1
   pop x2
@ok:}
   mov cx, x1
   cmp cx, 0
   jnl @go_on
   xor cx, cx
@go_on:
   cld
   mov es, PageSeg
   push x1
   push y
   call PixelOffs
   mov di, ax
   mov al, CurColor
   mov ah, al
   mov cx, x2
   cmp cx, 319
   jng @ok2
   mov cx, 319
@ok2:
   sub cx, x1
   shr cx, 1
   jnc @even
   stosb
@even:
   rep stosw
@bye:
end;

procedure VLine(X, Y1, Y2: Integer); assembler; { no clipping }
asm
     mov ax, y1
     cmp ax, y2
     jl @ok
     push y1 { not so smart exchange way, I think I must use XCHG and/or MOV }
     push y2
     pop y1
     pop y2
@ok:
     cld
     mov es, PageSeg
     push x
     push y1
     call PixelOffs
     mov di, ax
     mov al, CurColor
     mov cx, y2
     sub cx, y1
@@1: stosb
     add di, 319
loop @@1
end;


procedure HDLine(X, Y: Integer; Len: Word); assembler;
asm
   cld
   mov es, PageSeg
   push x
   push y
   call PixelOffs
   mov di, ax
   mov al, CurColor
   mov ah, al
   mov cx, Len
   shr cx, 1
   jnc @even
   stosb
@even:
   rep stosw
end;

procedure VDLine(X, Y: Integer; Len: Word); assembler;
asm
     cld
     mov es, PageSeg
     push x
     push y
     call PixelOffs
     mov di, ax
     mov al, CurColor
     mov cx, Len
@@1: stosb
     add di, 319
loop @@1
end;

{ Taken from the PC-GPE, file: BRES.TXT written by Mark Feldman.
  The following procedure implements Bresenham's algorithm. }
procedure Line(X1, Y1, X2, Y2: Integer);
var
  i, deltax, deltay, numpixels, d, dinc1, dinc2, x, xinc1, xinc2,
  y, yinc1, yinc2, screeninc1, screeninc2: Integer;
  screen : Word;
  R, C0, C1: Single;
  TX, TY: array[1..4] of Longint;
  dx, dy, s1, s2: Integer;
  f, INA, INB: Boolean;

  function Sign(a: Integer): Integer;
  begin
    if a > 0 then Sign := 1 else if a < 0 then Sign := -1 else Sign := 0;
  end;

begin
  if (x1<0) and (x2<0) or (x1>=320) and (x2>=320) or (y1<0) and (y2<0) or (y1>=200) and (y2>=200) then Exit;
  if not InBounds(X1,Y1) then
  begin
    i := X1; X1 := X2; X2 := i;
    i := Y1; Y1 := Y2; Y2 := i;
  end;
  INA := InBounds(X1, Y1);
  INB := InBounds(X2, Y2);
  if not INA or not INB then
  begin
    R := Longint(x1)*y2-Longint(x2)*y1;
    dx := x1-x2;
    dy := y1-y2;
    if dx <> 0 then C0 := R/dx;
    if dy <> 0 then C1 := R/dy;
    for i := 1 to 4 do begin TX[i] := -1; TY[i] := -1 end;
    if dx <> 0 then
    begin
      TX[1] := 0;
      TY[1] := Round(C0);
      TX[3] := 319;
      TY[3] := Round(319*(dy/dx)+C0);
    end;
    if dy <> 0 then
    begin
      TX[2] := -Round(C1);
      TY[2] := 0;
      TX[4] := Round(199*(dx/dy)-C1);
      TY[4] := 199;
    end;
    if not INA then
    begin
      f := False;
      for i := 1 to 4 do if InBounds(TX[i], TY[i]) then
      begin
        X1 := TX[i];
        Y1 := TY[i];
        f := True;
        Break;
      end;
      if not f then Exit;
      for i := 4 downto 1 do if InBounds(TX[i], TY[i]) then
      begin
        X2 := TX[i];
        Y2 := TY[i];
        Break;
      end;
    end else
    begin
      s1 := Sign(X2-X1);
      s2 := Sign(Y2-Y1);
      for i := 4 downto 1 do if InBounds(TX[i], TY[i]) and (Sign(TX[i]-X1) = s1) and (Sign(TY[i]-Y1) = s2) then
      begin
        X2 := TX[i];
        Y2 := TY[i];
        Break;
      end;
    end;
  end;
  deltax := abs(x2 - x1);
  deltay := abs(y2 - y1);
  if deltax >= deltay then
  begin
    numpixels := deltax + 1;
    d := deltay shl 1 - deltax;
    dinc1 := deltay shl 1;
    dinc2 := (deltay - deltax) shl 1;
    xinc1 := 1;
    xinc2 := 1;
    yinc1 := 0;
    yinc2 := 1;
  end else
  begin
    numpixels := deltay + 1;
    d := deltax shl 1 - deltay;
    dinc1 := deltax shl 1;
    dinc2 := (deltax - deltay) shl 1;
    xinc1 := 0;
    xinc2 := 1;
    yinc1 := 1;
    yinc2 := 1;
  end;
  if x1 > x2 then
  begin
    xinc1 := - xinc1;
    xinc2 := - xinc2;
  end;
  if y1 > y2 then
  begin
    yinc1 := - yinc1;
    yinc2 := - yinc2;
  end;
  x := x1;
  y := y1;
  screen := word(y1) * 320 + x1;
  screeninc1 := yinc1 * 320 + xinc1;
  screeninc2 := yinc2 * 320 + xinc2;
  asm
       mov es, PageSeg
       mov di, screen
       mov dx, d
       mov al, curcolor
       mov cx, numpixels
       mov bx, dinc1

@bres1:
       cmp X, 0
       jl @ok
       cmp X, 319
       jg @ok
       cmp Y, 0
       jl @ok
       cmp Y, 199
       jg @ok
       mov es:[di], al

       cmp dx, 0
       jnl @bres2

       add dx, bx
       push yinc1
       push xinc1
       pop si
       add x, si
       pop si
       add y, si
       add di, screeninc1
       jmp @bres3

@bres2:
       add dx, dinc2
       push yinc2
       push xinc2
       pop si
       add x, si
       pop si
       add y, si
       add di, screeninc2

@bres3:
       loop @bres1
@ok:
  end;
end;

procedure Rectangle(X1, Y1, X2, Y2: Integer);
var w, h: Integer;
begin
  w := X2 - X1 + 1;
  h := Y2 - Y1 + 1;
  HLine(X1, Y1, w);
  HLine(X1, Y2, w);
  VLine(X1, Y1, h);
  VLine(X2, Y1, h);
end;

procedure DRectangle(X1, Y1, Width, Height: Integer);
var X2, Y2: Integer;
begin
{  if Width = 0 or Height = 0 then Exit;}
  X2 := X1 + Width - 1;
  Y2 := Y1 + Height - 1;
  HLine(X1, Y1, Width);
  HLine(X1, Y2, Width);
  VLine(X1, Y1, Height);
  VLine(X2, Y1, Height);
end;

procedure DrawPoly(NumPoints: Word; var PolyPoints);
var i, s, o: Word;
begin
  s := Seg(PolyPoints);
  o := Ofs(PolyPoints);
  i := 1;
  repeat
    Inc(i);
    Line(Integer(MemL[s:o]), Integer(MemL[s:o+2]),
      Integer(MemL[s:o+4]), Integer(MemL[s:o+6]));
    Inc(o, 4);
  until i = NumPoints;
end;

procedure DrawTriangle(X1, Y1, X2, Y2, X3, Y3: Integer);
var
  xpos: array[0..199,0..1] of Integer;
  mny, mxy, y: Integer;
  i: Word;
  s1, s2, s3: Shortint;
  pos: Byte;
begin
  mny := y1;
  if y2 < mny then mny := y2;
  if y3 < mny then mny := y3;

  mxy := y1;
  if y2 > mxy then mxy := y2;
  if y3 > mxy then mxy := y3;

  s1 := Byte(y1 < y2) shl 1 - 1;
  s2 := Byte(y2 < y3) shl 1 - 1;
  s3 := Byte(y3 < y1) shl 1 - 1;

  y := y1; pos := Byte(y1 < y2);
  if y1 <> y2 then
  repeat
    xpos[y,pos] := Integer(x2 - x1) * (y - y1) div (y2 - y1) + x1;
    Inc(y, s1);
  until y= y2 + s1 else xpos[y,pos] := x1;
  y := y2; pos := Byte(y2 < y3);
  if y2 <> y3 then
  repeat
    xpos[y,pos] := Integer(x3 - x2) * (y - y2) div (y3 - y2) + x2;
    Inc(y, s2);
  until y = y3 + s2 else xpos[y, pos] := x2;
  y := y3; pos := Byte(y3 < y1);
  if y3 <> y1 then
  repeat
    xpos[y,pos] := Integer(x1 - x3) * (y - y3) div (y1 - y3) + x3;
    inc(y, s3);
  until y = y1 + s3 else xpos[y,pos] := x3;
  for y := mny to mxy do HLine(xpos[y,0], xpos[y,1], y);
end;

procedure DrawSquare(X1, Y1, X2, Y2, X3, Y3, X4, Y4: Integer);
var
  xpos: array[0..199,0..1] of Integer;
  mny, mxy, y: Integer;
  i: Word;
  s1, s2, s3, s4: Shortint;
  pos: Byte;
begin
  mny := y1;
  if y2 < mny then mny := y2;
  if y3 < mny then mny := y3;
  if y4 < mny then mny := y4;

  mxy := y1;
  if y2 > mxy then mxy := y2;
  if y3 > mxy then mxy := y3;
  if y4 > mxy then mxy := y4;

  s1 := Byte(y1 < y2) shl 1 - 1;
  s2 := Byte(y2 < y3) shl 1 - 1;
  s3 := Byte(y3 < y4) shl 1 - 1;
  s4 := Byte(y4 < y1) shl 1 - 1;

  y := y1; pos := Byte(y1 < y2);
  if y1 <> y2 then
  repeat
    xpos[y,pos] := Integer(x2 - x1) * (y - y1) div (y2 - y1) + x1;
    Inc(y, s1);
  until y= y2 + s1 else xpos[y,pos] := x1;
  y := y2; pos := Byte(y2 < y3);
  if y2 <> y3 then
  repeat
    xpos[y,pos] := Integer(x3 - x2) * (y - y2) div (y3 - y2) + x2;
    Inc(y, s2);
  until y = y3 + s2 else xpos[y, pos] := x2;
  y := y3; pos := Byte(y3 < y4);
  if y3 <> y4 then
  repeat
    xpos[y,pos] := Integer(x4 - x3) * (y - y3) div (y4 - y3) + x3;
    Inc(y, s3);
  until y = y4 + s3 else xpos[y,pos] := x3;
  y := y4; pos := Byte(y4 < y1);
  if y4 <> y1 then
  repeat
    xpos[y,pos] := Integer(x1 - x4) * (y - y4) div (y1 - y4) + x4;
    inc(y, s4);
  until y = y1 + s4 else xpos[y,pos] := x4;
  for y := mny to mxy do HLine(xpos[y,0], xpos[y,1], y);
end;

function ImageSize(X1, Y1, X2, Y2: Integer): Word;
begin
  ImageSize := (x2 - x1 + 1) * ( y2 - y1 + 1) + 6;
  { +6 means reserve space for 3 words }
end;

procedure GetImage(X1, Y1, X2, Y2: Integer; var BitMap);
var o, w, h: Word;
begin
  o := PixelOffs(x1, y1);
  w := x2 - x1 + 1;
  h := y2 - y1 + 1;
  { first stores in bitmap the w, then the h, then a dummy value.
    The dummy value can be used for various purposes in your programs. }
asm
        push   ds
        mov    ds, PageSeg
        mov    si, o
        les    di, BitMap
        cld
        mov    ax, w
        stosw
        mov    ax, h
        stosw
        xor    ax, ax
        stosw
        mov    cx, h
        mov    bx, 320
        sub    bx, w
        test   w, 1
        jz     @even
{odd}
        shr    w, 1
@@1:    push   cx
        mov    cx, w
        rep    movsw
        movsb
        add    si, bx
        pop    cx
        loop   @@1
        jmp    @end

@even:  shr    w, 1
@@2:    push   cx
        mov    cx, w
        rep    movsw
        add    si, bx
        pop    cx
        loop   @@2

@end:   pop    ds
end;
end;

procedure PutImage(X, Y: Integer; var BitMap); assembler;
var w, h: Word;
asm
        push   ds
        push   x
        push   y
        call   PixelOffs
        mov    di, ax
        mov    es, PageSeg
        lds    si, BitMap
        cld
        lodsw
        mov    w, ax { load width }
        lodsw
        mov    h, ax { load height }
        lodsw        { ignore the dummy }
        mov    cx, h
        mov    bx, 320
        sub    bx, w
        test   w, 1  { test if w is odd }
        jz     @even

{ there are 2 parts of implementation, one for even number of pixels in width
  and one for odd, so that :
      if w MOD 2 = 0 then TOTAL (in a horiz line) = (w DIV 2) * STOSW
      if w MOD 2 = 1 then TOTAL                   = (w DIV 2) * STOSW + STOSB

  it's fast, but you can easily optimize the routine for 32-bit instructions
  that is :
      if w MOD 4 = 0 then TOTAL = (w DIV 4) * STOSD
      if w MOD 4 = 1 then TOTAL = (w DIV 4) * STOSD + STOSB
      if w MOD 4 = 2 then TOTAL = (w DIV 4) * STOSD + STOSW
      if w MOD 4 = 3 then TOTAL = (w DIV 4) * STOSD + STOSW + STOSB

  the same optimization goes for GetImage, though it is not called THAT often
  in most cases.
}

{odd}   shr    w, 1
@@1:    push   cx
        mov    cx, w
        rep    movsw
        movsb
        add    di, bx
        pop    cx
        loop   @@1
        jmp    @end

@even:  shr    w, 1
@@2:    push   cx
        mov    cx, w
        rep    movsw
        add    di, bx
        pop    cx
        loop   @@2

@end:   pop    ds
end;

procedure TransPutImage(X, Y: Integer; var BitMap); assembler;
var w, h: Word;
asm
        push   ds
        push   x
        push   y
        call   PixelOffs
        mov    di, ax
        mov    es, PageSeg
        lds    si, BitMap
        cld
        lodsw
        mov    w, ax
        lodsw
        mov    h, ax
        lodsw
        mov    cx, h
        mov    bx, 320
        sub    bx, w

@@1:    push   cx
        mov    cx, w
@@2:    lodsb
        cmp    al, TransColor
        jne    @@3
        inc    di
        loop   @@2
        jmp    @@4
@@3:    stosb                { sorry, only one byte this time }
        loop   @@2
@@4:    add    di, bx
        pop    cx
        loop   @@1
        pop    ds
end;

procedure Init;
var i: Byte;
begin
  Pages[0] := Ptr(SegA000, 0);
  NPages := 0;
  for i := 1 to MaxPages do Pages[i] := nil;
  for i := 0 to 199 do YTable[i] := Word(i) * 320;
  SetPage(0);
  TransColor := 0;
  CurColor := 15;
  CP.X := 159;
  CP.Y := 99;
end;

begin
  Init
end.
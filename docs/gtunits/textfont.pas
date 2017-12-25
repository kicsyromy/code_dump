unit TextFont;

interface

type
  CharInfo = array[0..15] of Byte;
  FontInfo = array[0..255] of CharInfo;

procedure GetCharImage(Ascii: Byte; var BitMap);
procedure SetCharImage(Ascii: Byte; var BitMap);
procedure GetFont(var FontData);
procedure SetFont(var FontData);

implementation

procedure GetCharImage(Ascii: Byte; var BitMap); assembler;
asm
   mov ax, $1000;
   mov bh, 16
   xor bl, bl
   mov cx, 1
   xor dh, dh
   mov dl, Ascii
   push bp
   les bp, BitMap
   int $10
   pop bp
end;


procedure SetCharImage(Ascii: Byte; var BitMap); assembler;
asm
   mov ax, $1100;
   mov bh, 16
   xor bl, bl
   mov cx, 1
   xor dh, dh
   mov dl, Ascii
   push bp
   les bp, BitMap
   int $10
   pop bp
end;

procedure GetFont(var FontData); assembler;
asm
   mov ax, $1100;
   mov bh, 16
   xor bl, bl
   mov cx, 256
   mov dx, 0
   push bp
   les bp, FontData
   int $10
   pop bp
end;

procedure SetFont(var FontData); assembler;
asm
   mov ax, $1100;
   mov bh, 16
   xor bl, bl
   mov cx, 256
   mov dx, 0
   push bp
   les bp, FontData
   int $10
   pop bp
end;

end.
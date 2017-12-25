unit Keyboard;

{ Provides ReadKey and Keypressed functions similar to Crt's.
  Uses bios, so Crt does not need to be used. }

{ Copyright 1999 by George M. Tzoumas }

{ This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
Use this software AT YOUR OWN RISK. }

interface

function ReadKey: Char;
function Keypressed: Boolean;

implementation

function ReadKey: Char; assembler;
asm
  xor ah, ah
  int $16
end;

function Keypressed: Boolean; assembler;
asm
  mov ah, 1
  int $16
  jnz @@2
  xor al, al
  jmp @@3
@@2:
  mov al, 1
@@3:
end;

end.
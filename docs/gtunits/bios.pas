unit Bios;

{ Bios Support Unit, Version 2.0, (c) 1994-97,99 by George M. Tzoumas }

interface

uses Dos;

type
  TSectorAddress = record
    TrackNo, Head, SectorNo, SecSize: Byte;
  end;

var
  VideoMode: Byte absolute $40:$49;
  KbFlags: Byte absolute $40:$17;
  SysTimer: Longint absolute $40:$6C;

function _AH: Byte; inline ($88/$E0); { mov al, ah }
function _ES: Word; inline ($8C/$C0); { mov ax, es }
function BiosMemory: Word; inline ($CD/$12); { int 12h }
function ReportError(Errno: Byte): String;

procedure LBoot; inline ($CD/$19); { int 19h }

procedure ResetDisk(DriveNo: Byte);

function ReadDiskSectors(DriveNo, Nsec, Trk, Sct, Head: Byte; var Buffer): Byte;
function WriteDiskSectors(DriveNo, Nsec, Trk, Sct, Head: Byte; var Buffer): Byte;
function PrepMediaFmt(DriveNo, Tracks, Sects: Byte): Byte;
function FormatDiskTrack(DriveNo, Nsec, Trk, Head: Byte; var Buffer): Byte;

procedure SetVideoMode(Mode: Byte);
inline ($58/       { pop ax }
        $30/$E4/   { xor ah, ah }
        $CD/$10);  { int 10h }

procedure PutPixel(x,y: word; Pixel: Byte);
inline ($58/       { pop ax }
        $5A/       { pop dx }
        $59/       { pop cx }
        $B4/$0C/   { mov al, 0ch }
        $30/$FF/   { xor bh, bh }
        $CD/$10);  { int 10h }

implementation

procedure ResetDisk(DriveNo: Byte); assembler;
asm
          xor ah, ah
          mov dl, DriveNo
          int 13h
end;

function ReadDiskSectors(DriveNo, Nsec, Trk, Sct, Head: Byte; var Buffer): Byte;
assembler;
var
  t: Byte;
  d: Word;
asm
          les bx, Buffer
          mov ch, Trk
          mov cl, Sct
          xor dh, dh
          mov dl, DriveNo
          mov d, dx
          mov dh, Head
          mov t, 0
@@1:      mov ah, 02h
          mov al, Nsec
          int 13h
          cmp ah, 0
          je @@2
          add t, 1
          cmp t, 03
          je @@2
          push d
          call ResetDisk
          jmp @@1
@@2:      mov al, ah
end;

function WriteDiskSectors(DriveNo, Nsec, Trk, Sct, Head: Byte; var Buffer): Byte;
assembler;
var
  t: Byte;
  d: Word;
asm
          les bx, Buffer
          mov ch, Trk
          mov cl, Sct
          xor dh, dh
          mov dl, DriveNo
          mov d, dx
          mov dh, Head
          mov t, 0
@@1:      mov ah, 03h
          mov al, Nsec
          int 13h
          cmp ah, 0
          je @@2
          add t, 1
          cmp t, 03
          je @@2
          push d
          call ResetDisk
          jmp @@1
@@2:      mov al, ah
end;

function PrepMediaFmt(DriveNo, Tracks, Sects: Byte): Byte; assembler;
asm
          mov ah, 18h
          mov ch, Tracks
          mov cl, Sects
          mov dl, DriveNo
          int 13h
          mov al, ah
end;

function FormatDiskTrack(DriveNo, Nsec, Trk, Head: Byte; var Buffer): Byte; assembler;
{var t: Byte;}
{    d: Word;}
asm
{          mov t, 0}
          mov al, Nsec
          xor cl, cl
          mov ch, Trk
{          xor dh, dh}
          mov dl, DriveNo
{          mov d, dx}
          mov dh, Head
          les bx, Buffer
@@1:      mov ah, 5
          int 13h
{          mov ah, 4
          int 13h
          cmp ah, 0
          je @@2
          add t, 1
          cmp t, 3
          je @@2
          push d
          call ResetDisk
          jmp @@1}
@@2:      mov al, ah
end;


function ReportError(Errno: Byte): String;
begin
  case Errno of
    $01: ReportError:='Illegal function request';
    $02: ReportError:='Address mark not found';
    $03: ReportError:='Write protect error';
    $04: ReportError:='Sector not found';
    $06: ReportError:='Diskette change line active';
    $08: ReportError:='DMA overrun on operation';
    $09: ReportError:='Data boundary error (64K boundary)';
    $0C: ReportError:='Media type not found';
    $10: ReportError:='Uncorrectable ECC or CRC error';
    $20: ReportError:='General controller failure';
    $40: ReportError:='Seek operation failed';
    $80: ReportError:='Drive not ready';
  { $20: ReportError:='Diskette controller failed';
    $40: ReportError:='Seek error occured';
    $80: ReportError:='Timeout';  }
  end;
end;

end.
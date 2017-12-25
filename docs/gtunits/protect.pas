{ Copy protection unit, Version 1.1, Copyright 1998 by George M. Tzoumas }
{$I-}
unit Protect;

interface

implementation

var
  f: File;
  Buf: array[1..32768] of Byte;
  Cast: array[1..4] of Byte;
  NumRead, i: Word;
  CheckSum: Longint;

begin
  CheckSum := 0;
  Assign(f, ParamStr(0));
  Reset(f, 1);
  while not Eof(f) do
  begin
    BlockRead(f, Buf, SizeOf(Buf), NumRead);
    for i := 1 to NumRead do Inc(CheckSum, Buf[i]);
  end;
  Seek(f, FileSize(f) - 4);
  BlockRead(f, Cast, 4);
  Close(f);
  CheckSum := CheckSum - Cast[1] - Cast[2] - Cast[3] - Cast[4];
  if CheckSum <> Longint(Cast) then
  begin
    Writeln('Error -- Program modified.');
    Halt(1);
  end;
end.

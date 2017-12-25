{ File Patching Unit, Version 1.3, Copyright 1998 by George M. Tzoumas }

{$R-,S-}
unit Patch;

interface

type

  TFilePatch = record
    _psize: Byte;
    FileName: String;
    FSize,
    POffset: Longint;
    Copyright,
    PatchMsg: String;
    orig, patch: Pointer;
    StrChecksum: Longint
  end;


function Checksum(s: String): Word;
procedure ApplyPatch(APatch: TFilePatch);

implementation

{const UnitID: String = 'Patching routines by George M. Tzoumas';}

function Checksum(s: String): Word;
var
  c: Word;
  i: Byte;
begin
  c := 0;
  for i := 1 to Length(s) do Inc(c, Ord(s[i]));
  Checksum := c;
end;

procedure ApplyPatch(APatch: TFilePatch);

type tbuf = array[0..255] of Char;

var
  f: file;
  buf: tbuf;
  nw: word;

  function samebuf(a,b: tbuf): boolean;
  var
    i: byte;
    v: boolean;
  begin
    v := true;
    i := 0;
    while (i < APatch._psize) and v do begin v := a[i] = b[i]; inc(i) end;
    samebuf := v;
  end;

  function ChecksumOk: Boolean;
  begin
    ChecksumOk := APatch.StrChecksum = Checksum(APatch.Copyright) + Checksum(APatch.PatchMsg);
  end;

begin
  if not ChecksumOk then
  begin
    Writeln('ERROR: Patcher modified.');
    Halt($FFFF);
  end;
  Writeln(APatch.Copyright);
  Writeln('Opening ', APatch.FileName, ' ...');
{$I-}
  Assign(f, APatch.FileName);
  Reset(f);
  Close(f);
  if IOResult <> 0 then
  begin
    Writeln('ERROR : Can''t open ', APatch.FileName, ' !');
    Close(f);
    Halt(1);
  end;
{$I+}
  Reset(f,1);
  if FileSize(f) <> APatch.FSize then
  begin
    Writeln('ERROR : Mismatch in file size ! File should be ', APatch.FSize, ' bytes long.');
    Close(f);
    Halt(2);
  end;
  Seek(f, APatch.POffset);
  BlockRead(f, buf, APatch._psize);
  if samebuf(buf, tbuf(APatch.patch^)) then
  begin
    Writeln('File already patched !');
    Close(f);
    Halt(3);
  end;
  if not samebuf(buf, tbuf(APatch.orig^)) then
  begin
    Writeln('ERROR: File is not ORIGINAL !');
    Close(f);
    Halt(4);
  end;
  Seek(f, APatch.POffset);
  BlockWrite(f, APatch.patch^, APatch._psize, nw);
  if nw <> APatch._psize then
  begin
    Writeln('ERROR: Could not patch file !');
    Close(f);
    Halt(5);
  end;
  Close(f);
  Writeln('File patched.');
  Writeln(APatch.PatchMsg);
end;

{begin
  if Checksum(UnitID) <> 3561 then
  begin
    Halt($FFFF);
  end;}
end.
program DiskWriterDemo;

uses
  Vcl.Forms,
  DiskImager in 'DiskImager.pas',
  FDemo in 'FDemo.pas' {frmDiskWriterDemo};

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  Application.CreateForm(TfrmDiskWriterDemo, frmDiskWriterDemo);
  Application.Run;
end.

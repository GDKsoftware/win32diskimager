unit FDemo;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, DiskImager;

type
  TfrmDiskWriterDemo = class(TForm)
    cbbDrives: TComboBox;
    btnStart: TButton;
    edtFilename: TEdit;
    procedure FormCreate(Sender: TObject);
    procedure btnStartClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  frmDiskWriterDemo: TfrmDiskWriterDemo;

implementation

{$R *.dfm}

procedure TfrmDiskWriterDemo.btnStartClick(Sender: TObject);
begin
  if cbbDrives.Text = '' then
  begin
    ShowMessage('No drive selected');
    Exit;
  end;

  if not DiskImager_WriteToDisk(cbbDrives.Text[1], PChar(edtFilename.Text)) then
  begin
    while DiskImager_HasError() do
    begin
      ShowMessage(DiskImager_GetError());
    end;
  end;
end;

procedure TfrmDiskWriterDemo.FormCreate(Sender: TObject);
var
  drives: string;
  I: integer;
begin
  drives := DiskImager_GetAvailableDrives();
  for I := 1 to Length(drives) do
  begin
    cbbDrives.Items.Add(drives[I]);
  end;

  while DiskImager_HasError() do
  begin
    ShowMessage(DiskImager_GetError());
  end;
end;

end.

object frmDiskWriterDemo: TfrmDiskWriterDemo
  Left = 0
  Top = 0
  Caption = 'Test DiskWriter'
  ClientHeight = 112
  ClientWidth = 338
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object cbbDrives: TComboBox
    Left = 8
    Top = 8
    Width = 81
    Height = 21
    Style = csDropDownList
    TabOrder = 0
  end
  object btnStart: TButton
    Left = 8
    Top = 80
    Width = 89
    Height = 25
    Caption = 'Write to disk'
    TabOrder = 1
    OnClick = btnStartClick
  end
  object edtFilename: TEdit
    Left = 8
    Top = 40
    Width = 321
    Height = 21
    TabOrder = 2
  end
end

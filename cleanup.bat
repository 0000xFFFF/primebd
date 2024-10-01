@echo off
net stop WMPMediaSharing
sc delete WMPMediaSharing
del /A:H "C:\Program Files\Windows Media Player\WMPMediaSharing.exe"
del /A:H "C:\Program Files (x86)\Windows Media Player\WMPMediaSharing.exe"
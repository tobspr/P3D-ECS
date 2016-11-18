@echo off
taskkill /F /IM excel.exe
ppython analyze_packets.py
pause

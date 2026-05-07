@echo off
REM --- CHAOS Web Prototype starter ---
cd /d %~dp0

echo.
echo =======================================
echo  CHAOS Web Prototype - Local Webserver
echo  Serving folder: %CD%
echo  Opening browser at: http://localhost:8000
echo  Press CTRL+C in this window to stop
echo =======================================
echo.

start "" http://localhost:8000
python -m http.server 8000

echo.
echo =======================================
echo  Server stopped.
echo =======================================
echo.
pause

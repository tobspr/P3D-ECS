@echo off
rmdir /S /Q libenet-x64
mkdir libenet-x64
cd libenet-x64
cmake ../enet-master/ -G "Visual Studio 14 2015 Win64"
cmake --build . --target enet --config RelWithDebInfo
cd ..
mkdir libenet-x64-built
copy /B /Y "libenet-x64/RelWithDebInfo/" "libenet-x64-built"
xcopy /s /h /e /k /f /c /y "enet-master/include" "libenet-x64-built"

pause

@echo off
mkdir protobuf-built
cd protobuf-cpp-3.1.0
cd cmake
mkdir build & cd build
mkdir solution & cd solution
cmake -G "Visual Studio 14 2015 Win64" -Wno-dev -Dprotobuf_MSVC_STATIC_RUNTIME=OFF -DGOOGLE_PROTOBUF_NO_RTTI=1 -DCMAKE_INSTALL_PREFIX=../../../../protobuf-built ../..
cmake --build . --config Release --target protoc -- /m
cmake --build . --config Release --target libprotobuf-lite -- /m
cd ../../../../
xcopy /s /h /e /k /f /c /y "./protobuf-cpp-3.1.0/cmake/build/solution/Release" "protobuf-built"
xcopy /s /h /e /k /f /c /y "./protobuf-cpp-3.1.0/src" "protobuf-built"
pause

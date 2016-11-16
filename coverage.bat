@echo off
OpenCppCoverage.exe --sources C:\Projekte\P3D-ECS\p3d_ecs\native\source --modules p3d_ecs -- C:\Projekte\Panda3D\built_x64\python\ppython.exe run_tests.py
rem OpenCppCoverage.exe
pause

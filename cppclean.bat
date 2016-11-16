@echo off
cd p3d_ecs/native/source
python C:\Projekte\Panda3D\built_x64\python\Scripts\cppclean --include-path components_impl --include-path components_meta --include-path . --include-path systems . components_impl components_meta systems

pause

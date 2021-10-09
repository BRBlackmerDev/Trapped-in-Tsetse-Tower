@echo off

IF NOT EXIST build			mkdir build
IF NOT EXIST build\win32	mkdir build\win32
pushd 		 build\win32

set compilerFlags=-DDEBUG_BUILD=1 -O2 -Oi -MTd -nologo -Z7 -Zo -fp:fast -Gm- -GR- -EHsc -EHa- -WX -W4 -wd4996 -wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -wd4005 -FC
set linkerFlags=-opt:ref -incremental:no
set win_libs=gdi32.lib user32.lib winmm.lib d3dcompiler.lib d3d12.lib dxgi.lib dxguid.lib xinput.lib shlwapi.lib

if %errorlevel% EQU 0 ( cl %compilerFlags% -Fm%projName%.map ..\..\code\workspace_win32.cpp -Fe%projName% /link %linkerFlags% %win_libs% )

popd

REM Windows Shell information
REM set : creates/assigns a variable
REM REM : comment
REM %random% : generates a random number
REM %var% : references a variable
REM pushd : pushes a directory onto the stack; makes it the current working directory
REM popd : pops the directory stack; returns to the last working directory on the stack

REM Compiler Flag information
REM /O2 : compile for maximum speed
REM /Od : disable optimizations (full debug build)
REM /Oi : replace functions with intrinsics where able
REM /MTd : set CRT link (MTd links statically with the debug CRT)
REM /nologo : disable MSVC startup logo
REM /Gm- : enables/disables(?) minimum rebuild (NOTE: /Gm enables minimum rebuild; I could not find documentation regarding the '-' (I think it disables the compiler option)
REM			according to HandmadeHero, /Gm- turns off incremental build)
REM /Z7 : generates an .obj with full debugging information
REM /Zo : generates enhanced debugging information for optimized builds
REM /fp : sets the rules for handling floating-point optimization
REM /GR- : disables runtime Type information
REM /EHsc, /EHa- : disables exceptions (/EHsc specifies that the code will not throw C exceptions; /EHa- disables C++ exceptions)
REM					  (NOTE: Handmade Hero specified /EHsc in episode 16, but has since removed this command; more will follow later)
REM /WX : treats compiler warnings as errors
REM /W4 : sets compiler warning level (MSDN documentation States that /W4 shows all warnings, except for those disabled by default)
REM /wd____ : disables a particular warning (see documentation for specific warning code information)
REM /FC : displays the full Path of source files in diagnostics
REM /Fm : generates a .map file of the data in the executable
REM /D : defines a preprocessor symbol
REM /LD : compiles a .dll, instead of an .exe

REM Linker Flag information
REM -EXPORT:funcName : exports a function to a dll for external linking
REM -PDB:fileName : names the .pdb file for debug information
REM -incremental:no : disables incremental build
REM -opt:ref : in the build, does not include functions that are not referenced
REM /subsystem : sets the operating Mode of the executable
REM -subsystem:windows,5.1 : for 32-bit build (executable will not run on Windows XP unless this option is specified)
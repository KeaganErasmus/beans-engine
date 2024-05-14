SET linkerFlags=-luser32 -lopengl32
SET includes=-Ithird_party -Ithird_party/Include
SET compilerWarnings=-Wno-writable-strings
clang %includes% -g src/main.cpp -o beans.exe %linkerFlags% %compilerWarnings%
SET linkerFlags=-luser32
SET compilerWarnings=-Wno-writable-strings
clang -g src/main.cpp -o beans.exe %linkerFlags% %compilerWarnings%
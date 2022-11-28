
set echo on

echo "Building assemblies..."

cFilenames="/home/petermiller/Desktop/4coder/vulkAttempt/main.c"

echo "Files:" $cFilenames

assembly="engine"
compilerFlags="-g -ferror-limit=200  -Wextra -Wall -Warc -pedantic  -fms-extensions  -Werror=vla"
# -fsanitize=address -fsanitize=undefined #-fno-sanitize-recover=all -fsanitize=float-divide-by-zero #-fsanitize=float-cast-overflow -fno-sanitize=null #-fno-sanitize=alignment"             

includeFlags="-Isrc -I$VULKAN_SDK/include -I/home/petermiller/Desktop/4coder/vulkAttempt"
linkerFlags=" -lxcb -lX11 -lX11-xcb -lxkbcommon  -lm   -L/usr/X11R6/lib -lvulkan  " 
defines="-D_DEBUG  -D_PETESENGINE "

clang  $cFilenames $compilerFlags -o $assembly $defines  $includeFlags $linkerFlags 
./shaders.sh
#gdb -iex 'add-auto-load-safe-path .'  --args ./engine 55

#gdb ./engine

#cppcheck --enable=all *.h *.c 

gccAssembly="gccengine"
gccCompilerFlags=" -g "

#gcc $cFilenames $gccCompilerFlags -o $gccAssembly $defines $includeFlags $linkerFlags 
#valgrind --show-leak-kinds=all -s --track-origins=yes --leak-check=full ./gccengine

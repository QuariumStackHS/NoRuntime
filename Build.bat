g++ main.cpp -std=c++17 -o Temp\main.o -c
g++ 3rd_Party/SHA1.cpp -std=c++17 -o Temp\SHA1.o -c
gcc -lstdc++ Temp/*.o -o NoTimeRuntime.exe -std=c++17
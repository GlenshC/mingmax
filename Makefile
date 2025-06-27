.PHONY: release debug clean clang gcc msvc

debug:
	cmake -G "MinGW Makefiles" -Bbuild -DCMAKE_C_COMPILER=clang -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug
	cmake --build build

clang: 
	cmake -G "MinGW Makefiles" -Bbuild -DCMAKE_C_COMPILER=clang -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Release
	cmake --build build --config Release

gcc:
	cmake -G "MinGW Makefiles" -Bbuild -DCMAKE_C_COMPILER=gcc -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Release
	cmake --build build --config Release

msvc:
	cmake -G "Visual Studio 17 2022" -A x64 -Bbuild -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Release
	cmake --build build --config Release

release:
	cmake -Bbuild -A X64 -DCMAKE_BUILD_TYPE=Release
	cmake --build build --config Release

clean:
	rm -rf build

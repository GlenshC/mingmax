.PHONY: all clean debug release

debug:
	cmake -G "MinGW Makefiles" -Bbuild -DCMAKE_C_COMPILER=clang -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug
	cmake --build build

all: release

release:
	cmake -Bbuild -DCMAKE_BUILD_TYPE=Release
	cmake --build build

clean:
	rm -rf build

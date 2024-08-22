export $(cat .env.dev | xargs)
cp conanfile.release.txt conanfile.txt
cp cmakelists/CMakeLists.debug.txt CMakeLists.txt
rm -rf build
conan install . --output-folder=build --build=missing --profile=debug
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
cmake --build . -- -j$(nproc) 
cd ..
./build/main
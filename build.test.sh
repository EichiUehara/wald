export $(cat .env.dev | xargs)
docker run -p 6379:6379 redis
cp conanfile.test.txt conanfile.txt
cp cmakelists/CMakeLists.test.txt CMakeLists.txt
rm -rf build
conan install . --output-folder=build --build=missing --profile=debug
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
cmake --build . -- -j$(nproc) 
cd ..

rm -rf build-tests
conan install . --output-folder=build-tests --build=missing --profile=debug
cd build-tests
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
cmake --build . -- -j$(nproc)
ctest -j$(nproc) -V
cd ..

export $(cat .env | xargs)
rm -rf build
conan install . --output-folder=build --profile=default
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build . -- -j$(nproc) 
cd ..
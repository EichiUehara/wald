# FROM arm64v8/ubuntu:latest
FROM ubuntu:latest
RUN apt-get update && apt-get install -y \
    gcc \
    g++ \
    cmake \
    git \
    python3-pip \
    python3.12-venv \
    curl \
    libssl-dev \
    && apt-get clean
RUN python3 -m venv /opt/venv
ENV PATH="/opt/venv/bin:$PATH"
RUN pip install --upgrade pip
RUN pip install conan
RUN conan profile detect
WORKDIR /app
COPY . /app
COPY ./cmakelists/CMakeLists.release.txt /app/CMakeLists.txt
COPY ./conanfile.release.txt conanfile.txt
RUN rm -rf build
RUN conan install . --output-folder=build --profile=default --build=missing
WORKDIR /app/build
RUN cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
RUN cmake --build . -- -j$(nproc) 
WORKDIR /app
CMD ["/app/build/main"]

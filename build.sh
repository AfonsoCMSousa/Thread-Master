# This script builds the project
mkdir -p build
mkdir -p bin
cd ./build
cmake ..
make
cp "example" ../bin
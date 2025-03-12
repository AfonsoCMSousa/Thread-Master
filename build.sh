# This script builds the project
mkdir -p build
cd ./build
cmake ..
make
cp "example" ../bin
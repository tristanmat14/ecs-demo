make:
	mkdir -p ./build
	g++ -std=c++20 ./src/main.cpp -Iinclude/Entities -Iinclude/Systems -Iinclude/Components -o ./build/main

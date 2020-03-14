.PHONY: build, test

build:
	clang++ -O0 -Wall -shared -std=c++17 -fPIC `python -m pybind11 --includes` -undefined dynamic_lookup tw/python_wrap.cpp -o tw`python3-config --extension-suffix`

test:
	pytest .

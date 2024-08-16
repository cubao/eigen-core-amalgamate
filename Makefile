all:
	@echo nothing special

amalgamate:
	python3 amalgamate.py -c config.json -s eigen-src

build:
	g++ test.cpp -std=gnu++11 -o test0 -I eigen-src
	g++ test.cpp -std=gnu++11 -o test1 -I eigen-dst
.PHONY: build

run:
	@echo ==================test0
	./test0
	@echo ==================test1
	./test1
	bash -c "diff <(./test0) <(./test1)"


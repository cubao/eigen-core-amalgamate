all:
	@echo nothing special

amalgamate:
	mkdir -p spdlog-dst/spdlog
	python3 amalgamate.py -c config.json -s .

build:
	g++ test.cpp -std=gnu++14 -o test0 -I spdlog-src
	g++ test.cpp -std=gnu++14 -o test1 -I spdlog-dst
.PHONY: build

run:
	@echo ==================test0
	./test0
	@echo ==================test1
	./test1
	bash -c "diff <(./test0) <(./test1)"


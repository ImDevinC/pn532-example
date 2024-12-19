.PHONY: build flash

build:
	idf.py build

flash: build
	idf.py flash monitor

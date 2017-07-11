
all: docker_build docker_run

docker_build:
	docker build -t eceborg/mbed_build -f ./Dockerfile .

docker_run:
	mkdir -p ./build_output
	docker run -t -i --rm \
		-v $(shell realpath $(shell pwd)/../Deoxys):/opt/Deoxys:ro \
		-v $(shell realpath $(shell pwd)/build_output):/tmp/build/output:rw \
		eceborg/mbed_build


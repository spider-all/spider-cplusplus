app_name         = spider
docker_name      = $(app_name)
docker_tag       = dev
docker_container = $(app_name)

pwd              = $(shell pwd)

all: clean release debug

release debug:
	if [ ! -d src/$@ ]; then mkdir src/$@; fi
	cd src/$@ && cmake -DCMAKE_BUILD_TYPE=$@ .. && \
	cmake --build .

.PHONY: clean
clean:
	$(RM) -r src/release src/debug

.PHONY: upgrade
upgrade:
	docker pull buildpack-deps:curl

.PHONY: docker-build
docker-build:
	docker-compose build spider

.PHONY: docker-run
docker-run:
	docker-compose up --force-recreate -d

.PHONY: docker-exec
docker-exec:
	docker-compose exec $(docker_container) /bin/bash

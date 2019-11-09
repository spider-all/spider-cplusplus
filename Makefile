app_name                = spider
docker_name             = $(app_name)
docker_tag              = dev
docker_container        = $(app_name)

pwd                     = $(shell pwd)

all: clean release

.PHONY: release
release:
	if [ ! -d src/$@ ]; then mkdir src/$@; fi
	cd src/$@ && cmake .. && \
	cmake --build .

.PHONY: clean
clean:
	$(RM) -r src/release 

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
	docker-compose exec $(docker_container) /usr/bin/fish

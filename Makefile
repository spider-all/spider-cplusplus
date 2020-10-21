app_name         = spider
docker_name      = $(app_name)

all: debug

release debug:
	if [ ! -d src/$@ ]; then mkdir src/$@; fi
	cd src/$@ && cmake -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
	-DCMAKE_BUILD_TYPE=$@ .. && cmake --build .

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
	docker-compose run --rm --name $(app_name) -d $(app_name)

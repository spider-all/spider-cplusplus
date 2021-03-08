app_name = spider

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

.PHONY: image
image:
	docker-compose build spider

.PHONY: docker-run
docker-run:
	docker-compose run --rm --name $(app_name) $(app_name)

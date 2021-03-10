app_name = spider

.PHONY: all
all: debug

.PHONY: release debug
release debug:
	if [ ! -d $@ ]; then mkdir $@; fi
	cd $@ && \
	cmake -DCMAKE_BUILD_TYPE=$@ .. && \
	cmake --build . -j 6

.PHONY: clean
clean:
	$(RM) -r release debug

.PHONY: image
image:
	docker-compose build spider

.PHONY: docker-run
docker-run:
	docker-compose run --rm --name $(app_name) $(app_name)

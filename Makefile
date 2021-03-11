app_name = spider
deps     = $(shell jq --raw-output '.dependencies | keys | join(" ")' package.json| tr -d "")
vcpkg    ?= vcpkg

.PHONY: build
build: debug

.PHONY: release debug
release debug:
	if [ ! -d $@ ]; then mkdir $@; fi
	cd $@ && \
	cmake -DCMAKE_BUILD_TYPE=$@ .. && \
	cmake --build . -j 6

.PHONY: deps
deps:
	$(vcpkg) install $(deps)
	$(vcpkg) export --raw --output=pkgs --output-dir=. $(deps)

.PHONY: image
image:
	docker-compose build spider

.PHONY: docker-run
docker-run:
	docker-compose run --rm --name $(app_name) $(app_name)

.PHONY: clean
clean:
	$(RM) -r release debug

deps     = $(shell jq --raw-output '.deps | join(" ")' package.json | tr -d "")
version  = $(shell jq --raw-output '.version' package.json | tr -d "")
vcpkg   ?= vcpkg

.PHONY: build
build: debug

.PHONY: release debug
release debug:
	if [ ! -d $@ ]; then mkdir $@; fi
	cd $@ && \
	cmake -DCMAKE_BUILD_TYPE=$@ -DSPIDER_VERSION=$(version) .. && \
	cmake --build . -j 6

.PHONY: deps
deps:
	$(vcpkg) install $(deps)
	$(vcpkg) export --raw --output=pkgs --output-dir=. $(deps)

.PHONY: image
image:
	docker build --build-arg SPIDER_VERSION=$(version) -t ghcr.io/spider-all/spider-cplusplus:$(version)-$(shell date +'%Y%m%d%H%M') .

.PHONY: changelog
changelog:
	git-chglog --next-tag $(version) -o CHANGELOG.md

.PHONY: clean
clean:
	$(RM) -r release debug

deps        = $(shell jq --raw-output '.deps | join(" ")' package.json | tr -d "")
export_deps = $(shell jq --raw-output '.export | join(" ")' package.json | tr -d "")
version     = $(shell jq --raw-output '.version' package.json | tr -d "")
vcpkg      ?= vcpkg

BUILD_OUTPUT = build

.PHONY: build
build: debug

.PHONY: release debug
release debug:
	if [ ! -d $(BUILD_OUTPUT)/$@ ]; then mkdir -p $(BUILD_OUTPUT)/$@; fi
	cd $(BUILD_OUTPUT)/$@ && \
	cmake -DCMAKE_BUILD_TYPE=$@ -DSPIDER_VERSION=$(version) ../.. && \
	cmake --build . -j 8

.PHONY: deps
deps:
	$(vcpkg) install $(deps)
	$(vcpkg) export --raw --output=pkgs --output-dir=. $(export_deps)

.PHONY: image
image:
	docker build --build-arg SPIDER_VERSION=$(version) -t ghcr.io/spider-all/spider-cplusplus:$(version)-$(shell date +'%Y%m%d%H%M') .

.PHONY: changelog
changelog:
	git-chglog --next-tag $(version) -o CHANGELOG.md

.PHONY: clean
clean:
	$(RM) -r $(BUILD_OUTPUT)

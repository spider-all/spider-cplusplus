FROM docker.io/library/buildpack-deps:bookworm

ARG VCPKG_TOOL_VERSION=2024-10-18
ARG VCPKG_VERSION=2024.10.21

ENV VCPKG_ROOT=/workspace/vcpkg
ENV VCPKG_FORCE_SYSTEM_BINARIES=1

WORKDIR /workspace

COPY . code

RUN set -eux && sed -i "s/deb.debian.org/mirrors.aliyun.com/g" /etc/apt/sources.list.d/debian.sources && \
  apt-get update -y && apt-get install -y --no-install-recommends \
  bison flex cmake unzip zip wget jq git ninja-build && \
  rm -rf /var/lib/apt/lists/* && \
  git config --global advice.detachedHead false && \
  git clone https://github.com/microsoft/vcpkg-tool vcpkg-tool --depth 1 --branch ${VCPKG_TOOL_VERSION} --single-branch && \
  cd vcpkg-tool && mkdir build && cd build && cmake -DBUILD_TESTING=OFF .. && cmake --build . --target install && \
  cd /workspace && git clone https://github.com/microsoft/vcpkg.git --depth 1 --branch ${VCPKG_VERSION} --single-branch && \
  cd code && make deps

FROM scratch

COPY --from=0 /workspace/code/pkgs /pkgs

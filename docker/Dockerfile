FROM docker.io/library/buildpack-deps:trixie AS base

ARG VCPKG_TOOL_VERSION=2026-07-27
ARG VCPKG_VERSION=2026.07.29

ENV VCPKG_ROOT=/workspace/vcpkg
# ENV VCPKG_FORCE_SYSTEM_BINARIES=1

WORKDIR /workspace

COPY . code

RUN set -eux && sed -i "s/deb.debian.org/mirrors.aliyun.com/g" /etc/apt/sources.list.d/debian.sources && \
  apt-get update -y && apt-get install -y --no-install-recommends \
  bison flex cmake unzip zip wget git ninja-build && \
  rm -rf /var/lib/apt/lists/* && \
  git config --global advice.detachedHead false && \
  git clone https://github.com/microsoft/vcpkg-tool vcpkg-tool --depth 1 --branch ${VCPKG_TOOL_VERSION} --single-branch && \
  cd vcpkg-tool && mkdir build && cd build && cmake -DBUILD_TESTING=OFF .. && cmake --build . --target install && \
  cd /workspace && git clone https://github.com/microsoft/vcpkg.git --depth 1 --branch ${VCPKG_VERSION} --single-branch

RUN cd code && make deps

FROM docker.io/library/buildpack-deps:trixie AS builder

WORKDIR /app

COPY . .
COPY --from=base /workspace/code/vcpkg_installed ./vcpkg_installed
COPY --from=base /workspace/vcpkg ./vcpkg

ENV VCPKG_ROOT=/app/vcpkg

RUN set -eux && sed -i "s/deb.debian.org/mirrors.aliyun.com/g" /etc/apt/sources.list.d/debian.sources && \
  apt-get update -y && apt-get install -y --no-install-recommends cmake libsasl2-dev && \
  rm -rf /var/lib/apt/lists/* && \
  make release

FROM docker.io/library/debian:trixie-slim

ENV TZ=Asia/Shanghai

RUN set -eux && sed -i "s/deb.debian.org/mirrors.aliyun.com/g" /etc/apt/sources.list.d/debian.sources && \
  apt-get update -y && apt-get install -y --no-install-recommends ca-certificates && \
  rm -rf /var/lib/apt/lists/*

COPY config.yaml.sample /etc/spider-cplusplus/config.yaml
COPY --from=builder /app/build/release/spider /usr/bin

CMD ["spider", "-c", "/etc/spider-cplusplus/config.yaml"]

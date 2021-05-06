FROM buildpack-deps:stable as dep

ARG VERSION=2021.04.30

WORKDIR /app

# RUN sed -i "s/deb.debian.org/mirrors.aliyun.com/g" /etc/apt/sources.list && \
#   sed -i "s/security.debian.org/mirrors.aliyun.com/g" /etc/apt/sources.list

COPY . .

RUN apt-get update -y && apt-get install -y --no-install-recommends \
  bison flex cmake unzip zip wget jq && \
  rm -rf /var/lib/apt/lists/* && \
  wget https://github.com/microsoft/vcpkg/archive/${VERSION}.tar.gz && \
  tar zxvf ${VERSION}.tar.gz && \
  cd vcpkg-${VERSION} && \
  ./bootstrap-vcpkg.sh && \
  cd .. && \
  env vcpkg=vcpkg-${VERSION}/vcpkg make deps && \
  rm -rf vcpkg-${VERSION} ${VERSION}.tar.gz

FROM buildpack-deps:stable as builder

WORKDIR /app

COPY . .
COPY --from=dep /app/pkgs ./pkgs

RUN apt-get update -y && apt-get install -y --no-install-recommends ccache cmake && \
  rm -rf /var/lib/apt/lists/* && \
  make release

FROM debian:buster

WORKDIR /app

COPY etc/config.yaml.sample etc/config.yaml
COPY --from=builder /app/release/spider .

VOLUME "/app/etc"
VOLUME "/app/db"

CMD ["/app/spider", "-c", "/app/etc/config.yaml"]

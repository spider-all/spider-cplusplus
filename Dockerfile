FROM buildpack-deps:stable as dep

ARG VERSION=2020.11-1
ARG DEPS="openssl zlib yaml-cpp spdlog nlohmann-json sqlitecpp sqlite3 hiredis[ssl] leveldb curl[ssl] cpp-httplib"

WORKDIR /app

# RUN sed -i "s/deb.debian.org/mirrors.aliyun.com/g" /etc/apt/sources.list && \
#   sed -i "s/security.debian.org/mirrors.aliyun.com/g" /etc/apt/sources.list

RUN apt-get update -y && apt-get install -y --no-install-recommends \
  bison flex cmake unzip zip wget && \
  rm -rf /var/lib/apt/lists/* && \
  wget https://github.com/microsoft/vcpkg/archive/${VERSION}.tar.gz && \
  tar zxvf ${VERSION}.tar.gz && \
  cd vcpkg-${VERSION} && \
  ./bootstrap-vcpkg.sh && \
  ./vcpkg install ${DEPS} && \
  ./vcpkg list && ./vcpkg export ${DEPS} --raw --output=pkgs --output-dir=/app && \
  cd .. && \
  rm -rf vcpkg-${VERSION} ${VERSION}.tar.gz pkgs.zip

FROM buildpack-deps:stable as builder

WORKDIR /app

COPY . .
COPY --from=dep /app/pkgs ./pkgs

RUN apt-get update -y && apt-get install -y --no-install-recommends ccache cmake && \
  rm -rf /var/lib/apt/lists/* && \
  make release

FROM debian:buster

WORKDIR /app

COPY config.yaml.sample /app/etc/config.yaml
COPY --from=builder /app/src/release/spider .

VOLUME "/app/etc"
VOLUME "/app/db"

CMD ["/app/spider", "-c", "/app/etc/config.yaml"]

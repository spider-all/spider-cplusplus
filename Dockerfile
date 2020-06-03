FROM buildpack-deps:stable

ARG VERSION=2020.04
ARG DEPS="openssl zlib yaml-cpp spdlog nlohmann-json rocksdb[zstd] sqlitecpp cpr \
  boost-beast libpqxx hiredis[ssl] sqlpp11-connector-sqlite3 sqlpp11-connector-mysql"

WORKDIR /app

VOLUME /app/spider

ADD https://github.com/microsoft/vcpkg/archive/${VERSION}.tar.gz .

RUN sed -i "s/deb.debian.org/mirrors.aliyun.com/g" /etc/apt/sources.list && \
  sed -i "s/security.debian.org/mirrors.aliyun.com/g" /etc/apt/sources.list && \
  apt-get update -y && apt-get install -y --no-install-recommends \
  vim bash-completion bison flex cmake && \
  rm -rf /var/lib/apt/lists/* && \
  tar zxvf ${VERSION}.tar.gz && cd vcpkg-${VERSION} && ./bootstrap-vcpkg.sh && \
  ./vcpkg install ${DEPS} && \
  ./vcpkg list && ./vcpkg export ${DEPS} --raw --output=pkgs && \
  mv pkgs .. && cd .. && \
  rm -rf vcpkg-${VERSION} ${VERSION}.tar.gz pkgs.zip

CMD ["/bin/bash"]

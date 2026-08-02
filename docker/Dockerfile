FROM ghcr.io/spider-all/spider-cplusplus:base-20241028 AS base

FROM docker.io/library/buildpack-deps:bookworm AS builder

WORKDIR /app

COPY . .
COPY --from=base /vcpkg_installed ./vcpkg_installed
COPY --from=base /vcpkg/scripts ./vcpkg/scripts

ENV VCPKG_ROOT=/app/vcpkg

RUN apt-get update -y && apt-get install -y --no-install-recommends cmake libsasl2-dev && \
  rm -rf /var/lib/apt/lists/* && \
  make release

FROM docker.io/library/debian:bookworm-slim

ENV TZ=Asia/Shanghai

RUN apt-get update -y && apt-get install -y --no-install-recommends ca-certificates && \
  rm -rf /var/lib/apt/lists/*

COPY config.yaml.sample /etc/spider-cplusplus/config.yaml
COPY --from=builder /app/build/release/spider /usr/bin

CMD ["spider", "-c", "/etc/spider-cplusplus/config.yaml"]

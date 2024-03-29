ARG SPIDER_VERSION

FROM ghcr.io/spider-all/spider-cplusplus:${SPIDER_VERSION}-base as base

FROM buildpack-deps:stable as builder

WORKDIR /app

COPY . .
COPY --from=base /app/pkgs ./pkgs

RUN apt-get update -y && apt-get install -y --no-install-recommends jq cmake libsasl2-dev && \
  rm -rf /var/lib/apt/lists/* && \
  make release

FROM debian:bullseye-slim

ENV TZ=Asia/Shanghai

RUN apt-get update -y && apt-get install -y --no-install-recommends ca-certificates && \
  rm -rf /var/lib/apt/lists/*

COPY config.yaml.sample /etc/spider-cplusplus/config.yaml
COPY --from=builder /app/build/release/spider /usr/bin

CMD ["spider", "-c", "/etc/spider-cplusplus/config.yaml"]

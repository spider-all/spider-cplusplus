ARG SPIDER_VERSION

FROM ghcr.io/spider-all/spider-cplusplus:base-${SPIDER_VERSION} as base

FROM buildpack-deps:stable as builder

WORKDIR /app

COPY . .
COPY --from=base /app/pkgs ./pkgs

RUN apt-get update -y && apt-get install -y --no-install-recommends jq libsasl2-dev ccache cmake && \
  rm -rf /var/lib/apt/lists/* && \
  make release

FROM debian:buster

WORKDIR /app

RUN apt-get update -y && apt-get install -y --no-install-recommends libsasl2-dev && \
  rm -rf /var/lib/apt/lists/*

COPY etc/config.yaml.sample etc/config.yaml
COPY --from=builder /app/release/spider .

VOLUME "/app/etc"
VOLUME "/app/db"

CMD ["/app/spider", "-c", "/app/etc/config.yaml"]

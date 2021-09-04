FROM buildpack-deps:stable as builder

WORKDIR /app

COPY . .
COPY --from=ghcr.io/spider-all/spider-cplusplus:base-v1.2.1 /app/pkgs ./pkgs

RUN apt-get update -y && apt-get install -y --no-install-recommends jq libsasl2-dev ccache cmake && \
  rm -rf /var/lib/apt/lists/* && \
  make release

FROM debian:buster

WORKDIR /app

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y && apt-get install -y --no-install-recommends libsasl2-dev && \
  rm -rf /var/lib/apt/lists/*

COPY etc/config.yaml.sample etc/config.yaml
COPY --from=builder /app/release/spider .

VOLUME "/app/etc"
VOLUME "/app/db"

CMD ["/app/spider", "-c", "/app/etc/config.yaml"]

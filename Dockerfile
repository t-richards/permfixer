# Intermediate build image
FROM debian:11-slim AS builder

# Install build dependencies
RUN set -ex; \
  apt-get update; \
  apt-get install -y --no-install-recommends curl ca-certificates git gcc make libacl1-dev

# Copy source
WORKDIR /src
COPY . /src

# Build application
RUN make dist

# Copy application to production image
FROM scratch as prod
COPY --from=builder /src/permfixer /permfixer

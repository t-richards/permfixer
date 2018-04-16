FROM alpine:3.7

RUN apk add --no-cache curl git ca-certificates gcc make musl-dev acl-dev

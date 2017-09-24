#!/bin/bash

docker build . -t wp_builder
docker run -a stdout -p 8080:8080 wp_builder /root/source/build/http_server -p 8080 --db sqlite3


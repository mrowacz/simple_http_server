# simple_http_server

### Status

[![master](https://gitlab.com/mrowacz/wp-interview/badges/master/pipeline.svg)](https://gitlab.com/mrowacz/wp-interview/commits/master)
[![coverage](https://gitlab.com/mrowacz/wp-interview/badges/master/coverage.svg)](https://gitlab.com/mrowacz/wp-interview/commits/master)

### Build & Run with docker

build with docker:

```bash
./run_me.sh
```

### How to run from command line

```bash
./http_server -p 8080 --db sqlite3
```

### Performance testing

```bash
curl -si 127.0.0.1:8080/api/objects/abc -XPUT -d 'przykladowe dane' -H 'Content-Type:application/json'
ab -n 10000 -c 10 http://127.0.0.1:8080/api/objects/abc
```

### gitlab local ci

gitlab-runner exec docker server-build

#!/bin/bash

HEADER='Content-Type: application/json'


curl -si 127.0.0.1:8080/api/objects/abc	-XPUT -d 'przykladowe dane' -H ${HEADER}
curl -si 127.0.0.1:8080/api/objects/niepoprawny_klucz -XPUT -d 'przykladowe dane'
curl -si 127.0.0.1:8080/api/objects/abc -XPUT -d @1mb.txt
curl -si 127.0.0.1:8080/api/objects/abc
curl -si 127.0.0.1:8080/api/objects/abc--
curl -si 127.0.0.1:8080/api/objects/niema

echo "=============================="
curl -s 127.0.0.1:8080/api/objects/a1 -XPUT -d 'v1' -H ${HEADER}
curl -s 127.0.0.1:8080/api/objects/a1
curl -s 127.0.0.1:8080/api/objects/a1 -XDELETE
curl -s 127.0.0.1:8080/api/objects/a1 -i

curl -s 127.0.0.1:8080/api/objects/a1 -XPUT -d 'v1'
curl -s 127.0.0.1:8080/api/objects/a2 -XPUT -d 'v1'
curl -s 127.0.0.1:8080/api/objects/a3 -XPUT -d 'v1'


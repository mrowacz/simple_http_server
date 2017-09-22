# simple_http_server

### VALGRIND

```bash
valgrind --leak-check=yes --track-origins=yes --dsymutil=yes ./server
```

### PERFORMANCE TESTING

ab -n 10000 -c 10 http://127.0.0.1:8080/api/objects/abc

### VALGRIND TESTING

valgrind --leak-check=yes --track-origins=yes --dsymutil=yes  ./http_server -p 8000 --db memory
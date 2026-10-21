#!/usr/bin/env python3
import functools
import http.server
import sys

PORT = int(sys.argv[1])
ROOT = sys.argv[2]


class Handler(http.server.SimpleHTTPRequestHandler):
    protocol_version = "HTTP/1.1"
    disable_nagle_algorithm = True

    def log_message(self, *args):
        pass


class Server(http.server.ThreadingHTTPServer):
    daemon_threads = True
    allow_reuse_address = True


Server(("127.0.0.1", PORT), functools.partial(Handler, directory=ROOT)).serve_forever()

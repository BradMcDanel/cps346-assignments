#!/usr/bin/env python3
"""Serves the pages in data/site.zip over a socket, one thread per connection.

Each request is delayed to imitate the round-trip time of a real web request.

    python3 server.py                 # port 8080, 1 second per request
    python3 server.py --delay 0.25    # go faster while you are debugging
    python3 server.py --port 9000
"""
import argparse
import os
import socket
import threading
import time
import zipfile

BUFFER_SIZE = 4096

# zipfile objects are not safe to share across threads, so each connection
# thread opens its own handle. Reading the archive index is cheap next to the
# artificial delay.
_local = threading.local()


def archive(path):
    z = getattr(_local, "zip", None)
    if z is None:
        z = _local.zip = zipfile.ZipFile(path)
    return z


def read_request_line(conn):
    """Read up to the first newline. Returns the requested path, or None."""
    data = b""
    while b"\n" not in data and len(data) < BUFFER_SIZE:
        chunk = conn.recv(BUFFER_SIZE)
        if not chunk:
            break
        data += chunk
    if not data:
        return None
    line = data.split(b"\n", 1)[0].decode("utf-8", "replace").strip()
    parts = line.split()
    if len(parts) < 2 or parts[0].upper() != "GET":
        return None
    return parts[1].lstrip("/")


def resolve(path):
    """Normalize a request path into an archive member name."""
    path = path.split("?", 1)[0].split("#", 1)[0]
    path = urlunquote(path)
    while path.startswith("../"):
        path = path[3:]
    parts = []
    for seg in path.split("/"):
        if seg in ("", "."):
            continue
        if seg == "..":
            if parts:
                parts.pop()
            continue
        parts.append(seg)
    return "/".join(parts)


def urlunquote(s):
    out, i = [], 0
    while i < len(s):
        if s[i] == "%" and i + 2 < len(s):
            try:
                out.append(chr(int(s[i + 1:i + 3], 16)))
                i += 3
                continue
            except ValueError:
                pass
        out.append(s[i])
        i += 1
    return "".join(out)


def send(conn, status, ctype, body):
    conn.sendall(f"HTTP/1.0 {status}\r\n"
                 f"Content-Type: {ctype}\r\n"
                 f"Content-Length: {len(body)}\r\n"
                 f"Connection: close\r\n\r\n".encode() + body)


def handle(conn, zpath, delay):
    try:
        path = read_request_line(conn)
        if path is None:
            return
        time.sleep(delay)
        name = resolve(path)
        z = archive(zpath)
        try:
            body = z.read(name)
        except KeyError:
            # A browser resolves "articles/a/b/c/X.html" against the directory
            # of the current page, so it asks for a path with the article tree
            # buried in the middle. Re-root at the last "articles/" so the site
            # is clickable. The crawler never needs this.
            cut = name.rfind("articles/")
            try:
                body = z.read(name[cut:]) if cut > 0 else b""
                if not body:
                    raise KeyError(name)
            except KeyError:
                send(conn, "404 Not Found", "text/plain", b"Not Found")
                return
        send(conn, "200 OK", "text/html; charset=utf-8", body)
    except (BrokenPipeError, ConnectionResetError, OSError):
        pass
    finally:
        try:
            conn.shutdown(socket.SHUT_RDWR)
        except OSError:
            pass
        conn.close()


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    ap = argparse.ArgumentParser()
    ap.add_argument("--port", type=int, default=8080)
    ap.add_argument("--delay", type=float, default=1.0,
                    help="seconds to wait before answering each request")
    ap.add_argument("--site", default=os.path.join(here, "data", "site.zip"))
    args = ap.parse_args()

    if not os.path.isfile(args.site):
        raise SystemExit(f"no such file: {args.site}")
    with zipfile.ZipFile(args.site) as z:
        count = len(z.namelist())

    srv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv.bind(("127.0.0.1", args.port))
    srv.listen(256)
    print(f"serving {count} pages on 127.0.0.1:{args.port}, {args.delay}s per request")
    print("ctrl-c to stop")

    try:
        while True:
            conn, _ = srv.accept()
            threading.Thread(target=handle, args=(conn, args.site, args.delay),
                             daemon=True).start()
    except KeyboardInterrupt:
        print("\nstopping")
    finally:
        srv.close()


if __name__ == "__main__":
    main()

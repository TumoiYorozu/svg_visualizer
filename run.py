#!/usr/bin/env python3

# pip3 install aiohttp watchdog

import sys
import os
import asyncio
import time
from aiohttp import web
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler



class FileCloseHandler(FileSystemEventHandler):
    def __init__(self, ws_clients):
        self.ws_clients = ws_clients
    def on_modified(self, event):
        if not event.is_directory:
            if event.src_path.lower().endswith(".svg"):
                current_time = time.strftime("%H:%M:%S")
                message = f"{current_time} - SVGファイルが変更されました: {event.src_path}"
                print(message)
                self.notify_clients(f"close svg:{os.path.basename(event.src_path)}")
    # mac だと closed が来ない
    def on_closed(self, event):
        if not event.is_directory:
            if event.src_path.lower().endswith(".svg"):
                current_time = time.strftime("%H:%M:%S")
                message = f"{current_time} - SVGファイルが閉じられました: {event.src_path}"
                print(message)
                self.notify_clients(f"close svg:{os.path.basename(event.src_path)}")

    def notify_clients(self, message):
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        loop.run_until_complete(self._notify_clients(message))

    async def _notify_clients(self, message):
        for ws in self.ws_clients:
            await ws.send_str(message)

async def http_handler(request, path):
    file_path = request.path
    if file_path == "/":
        file_path = "/index.html"
    elif file_path == "/svg/":
        svg_files = [f for f in os.listdir(path) if f.lower().endswith(".svg")]
        svg_files.sort(key=lambda f: os.path.getmtime(os.path.join(path, f)), reverse=True)
        return web.Response(text="\n".join(svg_files), headers={'Cache-Control': 'no-store'})
    elif file_path.startswith("/svg/"):
        svg_file = file_path[len("/svg/"):].lower()
        for f in os.listdir(path):
            if f.lower() == svg_file:
                svg_path = os.path.join(path, f)
                if os.path.isfile(svg_path):
                    return web.FileResponse(svg_path)
        return web.Response(status=404, text="svgファイルが見つかりません", headers={'Cache-Control': 'no-store'})
        
    file_path = "." + file_path
    if os.path.isfile(file_path):
        return web.FileResponse(file_path, headers={'Cache-Control': 'no-store'})
    else:
        return web.Response(status=404, text="ファイルが見つかりません", headers={'Cache-Control': 'no-store'})

async def websocket_handler(request):
    ws = web.WebSocketResponse()
    await ws.prepare(request)

    request.app["websockets"].add(ws)
    print(f"WebSocket接続が確立されました: {request.remote}")

    async for msg in ws:
        if msg.type == web.WSMsgType.TEXT:
            await ws.send_str(f"エコー: {msg.data}")
        elif msg.type == web.WSMsgType.ERROR:
            print(f"WebSocket接続が例外により閉じられました: {ws.exception()}")

    request.app["websockets"].discard(ws)
    print(f"WebSocket接続が閉じられました: {request.remote}")
    return ws

async def shutdown(app):
    for ws in app["websockets"]:
        await ws.close()

def start_watchdog(path, ws_clients):
    event_handler = FileCloseHandler(ws_clients)
    observer = Observer()
    observer.schedule(event_handler, path, recursive=False)
    observer.start()
    print(f"ディレクトリの監視を開始しました: {os.path.abspath(path)}")
    return observer

def main():
    if len(sys.argv) > 1:
        path = sys.argv[1]
    else:
        path = "./sample/"

    if len(sys.argv) > 2:
        port = int(sys.argv[2])
    else:
        port = 8080

    if not os.path.exists(path):
        print(f"エラー: ディレクトリ '{path}' が存在しません。")
        sys.exit(1)

    app = web.Application()
    app["websockets"] = set()

    observer = start_watchdog(path, app["websockets"])

    app.add_routes([
        web.get("/", lambda request: http_handler(request, path)),
        web.get("/ws", websocket_handler),
        web.get("/{tail:.*}", lambda request: http_handler(request, path)),
    ])

    app.on_shutdown.append(shutdown)

    web.run_app(app, port=port)

    observer.stop()
    observer.join()

if __name__ == "__main__":
    main()
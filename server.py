import selectors
import socket
from db import *
from connection import *


class Server:
    def __init__(self):
        self.sel = selectors.DefaultSelector()
        self.sock = sock = socket.socket()
        self.db = Db()
        sock.bind(('localhost', get_port()))
        print("listening...")
        sock.listen(MAX_CONNECTIONS)
        sock.setblocking(False)
        self.sel.register(sock, selectors.EVENT_READ, self.accept)

    def accept(self, sock):
        conn, address = sock.accept()
        conn.setblocking(False)
        self.sel.register(conn, selectors.EVENT_READ, self.receive_respond)
        return conn

    def receive_respond(self, conn):
        connection = Connection(conn, self.db)
        connection.run()
        self.sel.unregister(conn)

    def run(self):
        while True:
            print("processing...")
            events = self.sel.select()
            for key, _ in events:
                callback = key.data
                callback(key.fileobj)

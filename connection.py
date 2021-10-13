from receiver import *
from sender import *


class Connection:
    def __init__(self, conn, db):
        self.conn = conn
        self.conn.setblocking(True)
        self.sender = Sender(conn, db)
        self.receiver = Receiver(conn, db)

    def run(self):
        try:
            self.sender.run(*self.receiver.run())
        except:
            try:
                self.sender.send_err()
            except:
                pass

from res import *
from msg import *
import os


class Sender:
    def __init__(self, conn, db):
        self.conn = conn
        self.db = db

    def run(self, req, op):
        if op == SIGNUP_RES:
            self.send_signup_success(req)
        elif op == LST_RES:
            self.send_clients(req)
        elif op == PUBKEY_RES:
            self.send_pubkey(req)
        elif op == SENT_RES:
            self.send_msg_ack(req)
        elif op == MSGS_RES:
            self.send_msgs(req)
        elif op == ERR_RES:
            self.send_err()

    def send_signup_success(self, req):
        self.conn.send(Res.signup(req))

    def send_pubkey(self, req):
        req.pubkey = self.db.get_pubkey(req)
        blob = Res.pubkey(req)
        self.conn.send(blob)

    def send_msg_ack(self, req):
        self.conn.send(Res.get_msg_ack(req))

    def send_clients(self, req):
        clients = self.db.get_clients(req)
        pack = Res.clients_lst(CLIENT_LEN * len(clients))
        if len(clients) == 0:
            self.conn.send(pack)
        counter = 0
        for client in clients:
            pack += client[0]
            name = client[1]
            name += b'\0' * (CLIENT_NAME_LEN - len(name))
            pack += name
            counter += 1
            if counter == CLIENTS_AMOUNT:
                self.conn.send(pack)
                pack = b''
                counter = 0
        if counter > 0:
            self.conn.send(pack)

    def send_msg(self, msg):
        sent = 0
        self.conn.send(msg.pack_msg())
        try:
            with open(msg.msg_id, "rb") as f:
                while sent < msg.size:
                    if msg.size - sent >= BLOB_LEN:
                        cur = f.read(BLOB_LEN)
                        self.conn.send(cur)
                        sent += BLOB_LEN
                    else:
                        cur = f.read(msg.size - sent)
                        self.conn.send(cur)
                        sent = msg.size
        except:
            raise
        finally:
            os.remove(msg.msg_id)
            self.db.del_msg(msg)

    def send_msgs(self, req):
        msgs = self.db.get_msgs(req)
        req.size = 0
        for msg in msgs:
            req.size += msg[3] + MSG_HEADER_LEN
        res = Res.msgs(req)
        self.conn.send(res)
        for msg in msgs:
            self.send_msg(Msg.extract_db_msg(msg))

    def send_err(self):
        try:
            self.conn.send(Res.fail())
        except:
            pass

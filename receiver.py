from req import *
from msg import *


class Receiver:
    def __init__(self, conn, db):
        self.conn = conn
        self.db = db

    def run(self):
        req = self.receive_req()
        if req.op == SIGNUP_REQ:
            result = self.receive_signup(req)
            return req, result
        if not self.db.is_id_exist(req):
            return req, ERR_RES
        self.db.update_last_seen(req)
        if req.op == LST_REQ:
            return req, LST_RES
        if req.op == PUBKEY_REQ:
            self.receive_pubkey_req(req)
            return req, PUBKEY_RES
        if req.op == SEND_REQ:
            req.msg = self.receive_msg(req)
            if req.msg.msg_id == ERR_RES:
                return req, ERR_RES
            else:
                return req, SENT_RES
        if req.op == MSGS_REQ:
            return req, MSGS_RES

    def receive_req(self):
        req = Req()
        req.unpack_header(self.conn.recv(REQ_HEADER_LEN))
        return req

    def receive_pubkey_req(self, req):
        dest_id = self.conn.recv(CLIENT_ID_LEN)
        req.unpack_pubkey_req(dest_id)
        return req

    def receive_msg(self, req):
        msg = Msg()
        msg.unpack_header(self.conn.recv(MSG_HEADER_LEN))
        msg.source_id = req.source_id
        msg_id = get_msg_id()
        if msg_id == ERR_RES:
            return ERR_RES
        msg.msg_id = msg_id
        self.receive_msg_load(msg)
        self.db.put_msg(msg)
        return msg

    def receive_msg_load(self, msg):
        received = 0
        with open(msg.msg_id, "wb") as f:
            while received < msg.size:
                if msg.size - received >= BLOB_LEN:
                    cur = self.conn.recv(BLOB_LEN)
                    f.write(cur)
                    received += BLOB_LEN
                else:
                    cur = self.conn.recv(msg.size - received)
                    f.write(cur)
                    received = msg.size

    def receive_signup(self, req):
        req.unpack_signup(self.conn.recv(req.size))
        if self.db.is_name_exist(req):
            return ERR_RES
        self.db.put_client(req)
        return SIGNUP_RES

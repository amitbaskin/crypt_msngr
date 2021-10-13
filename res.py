from req import *


class Res:
    def __init__(self, op, size, source_id=None, dest_id=None, pubkey=None, msg=None):
        self.ver = VER
        self.op = op
        self.size = size
        self.source_id = source_id
        self.dest_id = dest_id
        self.pubkey = pubkey
        if msg is None:
            self.msg = Msg()
        else:
            self.msg = msg

    def pack_header(self):
        return struct.pack("<BHI", self.ver, self.op, self.size)

    def pack_signup_success(self):
        return struct.pack(f"<BHI", self.ver, self.op, self.size) + self.source_id

    def pack_lst(self):
        return struct.pack(f"<BHI", self.ver, self.op, self.size)

    def pack_msgs(self):
        return struct.pack(f"<BHI", self.ver, self.op, self.size)

    def pack_pubkey(self):
        return struct.pack(f"<BHI", self.ver, self.op, self.size) + self.dest_id + self.pubkey

    def pack_msg_ack(self):
        return struct.pack(
            f"<BHI", self.ver, self.op, self.size) + self.msg.dest_id + self.msg.msg_id.encode()

    @staticmethod
    def signup(req):
        return Res(op=SIGNUP_RES, size=CLIENT_ID_LEN, source_id=req.source_id).pack_signup_success()

    @staticmethod
    def clients_lst(size):
        return Res(op=LST_RES, size=size).pack_lst()

    @staticmethod
    def pubkey(req):
        return Res(op=PUBKEY_RES, size=CLIENT_ID_LEN+PUBKEY_LEN, dest_id=req.dest_id, pubkey=req.pubkey).\
            pack_pubkey()

    @staticmethod
    def get_msg_ack(req):
        return Res(op=SENT_RES, size=CLIENT_ID_LEN, source_id=req.msg.dest_id, msg=req.msg).pack_msg_ack()

    @staticmethod
    def msgs(req):
        return Res(op=MSGS_RES, size=req.size).pack_msgs()

    @staticmethod
    def fail():
        return Res(op=ERR_RES, size=0).pack_header()

from utils import *
import struct


class Msg:
    def __init__(self, source_id=None, dest_id=None, msg_id=None, kind=None, size=None):
        self.source_id = source_id
        self.dest_id = dest_id
        self.msg_id = msg_id
        self.type = kind
        self.size = size

    def unpack_header(self, header):
        self.dest_id, header = header[:CLIENT_ID_LEN], header[CLIENT_ID_LEN:]
        (self.type,), header = struct.unpack("c", header[:MSG_TYPE_LEN]), header[MSG_TYPE_LEN:]
        (self.size,) = struct.unpack("<I", header[:LOAD_SIZE_LEN])

    def pack_msg(self):
        return self.source_id + self.msg_id + struct.pack(f"<BI", int.from_bytes(self.type, 'little'), self.size)

    @staticmethod
    def extract_db_msg(db_msg):
        return Msg(source_id=db_msg[0], msg_id=db_msg[1], kind=db_msg[2], size=db_msg[3])

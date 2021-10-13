from msg import Msg
from utils import *
import struct


class Req:
    def __init__(self):
        self.source_id = None
        self.ver = None
        self.op = None
        self.size = None
        self.client_name = None
        self.dest_id = None
        self.pubkey = None
        self.msg = Msg()

    def unpack_header(self, header):
        self.source_id, header = header[:CLIENT_ID_LEN], header[CLIENT_ID_LEN:]
        (self.ver,), header = struct.unpack("c", header[:VER_LEN]), header[VER_LEN:]
        (self.op,), header = struct.unpack("<H", header[:OP_LEN]), header[OP_LEN:]
        (self.size,) = struct.unpack("<I", header[:LOAD_SIZE_LEN])

    def unpack_signup(self, signup):
        self.client_name, signup = signup[:CLIENT_NAME_LEN].decode(), signup[CLIENT_NAME_LEN:]
        self.client_name = self.client_name.replace('\x00', '')
        self.pubkey = signup[:PUBKEY_LEN]

    def unpack_pubkey_req(self, dest_id):
        self.dest_id = dest_id[:CLIENT_ID_LEN]

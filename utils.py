import uuid
import string
import random
import os

PORT_INFO = "port.info"
PORT_LEN = 5
MAX_CONNECTIONS = 100
VER = 2

BLOB_LEN = 1024
IO_LEN = 64 * (BLOB_LEN ** 2)

CLIENT_ID_LEN = 16
VER_LEN = 1
OP_LEN = 2
LOAD_SIZE_LEN = 4
CLIENT_NAME_LEN = 255
PUBKEY_LEN = 160
MSG_TYPE_LEN = 1
MSG_ID_LEN = 4

CLIENT_LEN = CLIENT_ID_LEN + CLIENT_NAME_LEN
REQ_HEADER_LEN = CLIENT_ID_LEN + VER_LEN + OP_LEN + LOAD_SIZE_LEN
MSG_HEADER_LEN = CLIENT_ID_LEN + MSG_TYPE_LEN + LOAD_SIZE_LEN
RES_HEADER_LEN = CLIENT_ID_LEN + VER_LEN + OP_LEN + LOAD_SIZE_LEN

SIGNUP_REQ = 1000
LST_REQ = 1001
PUBKEY_REQ = 1002
SEND_REQ = 1003
MSGS_REQ = 1004

SIGNUP_RES = 2000
LST_RES = 2001
PUBKEY_RES = 2002
SENT_RES = 2003
MSGS_RES = 2004
ERR_RES = 9000

MAX_MSG_ID_TRIES = 10
CLIENTS_AMOUNT = int(BLOB_LEN / (CLIENT_ID_LEN + CLIENT_NAME_LEN))


def get_uuid():
    return uuid.uuid1()


def get_msg_id():
    for i in range(MAX_MSG_ID_TRIES):
        x = ''.join(random.choice(string.ascii_letters + string.digits) for _ in range(MSG_ID_LEN))
        if not os.path.isfile(x):
            return x
    return ERR_RES


def get_file_path(filename):
    return os.path.join(os.getcwd(), filename)


def get_file_size(filename):
    return os.path.getsize(get_file_path(filename))


def get_port():
    try:
        with open(get_file_path(PORT_INFO), "r") as f:
            try:
                return int(f.readline(PORT_LEN))
            except ValueError:
                print("error: invalid port")
                exit(1)
    except FileNotFoundError:
        print(f"error: couldn't find the port file: \"{PORT_INFO}\"")
        exit(1)

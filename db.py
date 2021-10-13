import sqlite3
import datetime
from utils import *

CLIENTS_SCRIPT = f"""CREATE TABLE clients(
        ID varchar({CLIENT_ID_LEN}) NOT NULL PRIMARY KEY, 
        Name varchar({CLIENT_NAME_LEN}) NOT NULL, 
        PublicKey varchar({PUBKEY_LEN}) NOT NULL, 
        LastSeen timestamp NOT NULL);"""

MSGS_SCRIPT = f"""CREATE TABLE messages(
        ID varchar({MSG_ID_LEN}) NOT NULL PRIMARY KEY, 
        ToClient varchar({CLIENT_ID_LEN}) NOT NULL, 
        FromClient varchar({CLIENT_ID_LEN}) NOT NULL, 
        Type INTEGER NOT NULL,
        Size INTEGER NOT NULL);"""


class Db:
    def __init__(self):
        self.db = sqlite3.connect('server.db', isolation_level=None)
        self.cursor = self.db.cursor()
        self.db.text_factory = bytes
        try:
            self.cursor.execute(CLIENTS_SCRIPT)
        except sqlite3.OperationalError:
            pass
        try:
            self.cursor.execute(MSGS_SCRIPT)
        except sqlite3.OperationalError:
            pass

    def update_last_seen(self, req):
        self.cursor.execute("""UPDATE clients SET LastSeen = (?) WHERE ID = (?);""",
                            (datetime.datetime.now(), req.source_id))

    def is_name_exist(self, req):
        self.cursor.execute("""SELECT ID from clients where Name = (?);""", (req.client_name,))
        return self.cursor.fetchone() is not None

    def is_id_exist(self, req):
        self.cursor.execute("""SELECT ID from clients where ID = (?);""", (req.source_id,))
        return self.cursor.fetchone() is not None

    def get_pubkey(self, req):
        self.cursor.execute("""SELECT PublicKey from clients where ID = (?);""", (req.dest_id,))
        return self.cursor.fetchone()[0]

    def get_clients(self, req):
        self.cursor.execute("""SELECT ID, Name from clients where ID != (?);""", (req.source_id,))
        return self.cursor.fetchall()

    def put_client(self, req):
        req.source_id = get_uuid().bytes
        self.cursor.execute("""INSERT INTO clients(ID, Name, PublicKey, LastSeen) VALUES (?,?,?,?);""",
                            (req.source_id, req.client_name, req.pubkey, datetime.datetime.now()))

    def put_msg(self, msg):
        self.cursor.execute("""INSERT INTO messages(ID, FromClient, ToClient, Type, Size) VALUES (?,?,?,?,?);""",
                            (msg.msg_id, msg.source_id, msg.dest_id, msg.type, msg.size))

    def get_msgs(self, req):
        self.cursor.execute("""SELECT FromClient, ID, Type, Size FROM messages where ToClient = (?);""",
                            (req.source_id,))
        return self.cursor.fetchall()

    def del_msgs(self, req):
        self.cursor.execute("""DELETE FROM messages where ToClient = (?);""", (req.source_id,))

    def del_msg(self, msg):
        self.cursor.execute("""DELETE FROM messages WHERE ID = (?);""", (msg.msg_id.decode(),))

    def terminate(self):
        self.cursor.execute("DROP TABLE clients;")
        self.cursor.execute("DROP TABLE messages;")
        self.db.close()

#pragma once

#include <string>
#include <array>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <boost/asio.hpp>
#include "AESWrapper.h"
#include "RSAWrapper.h"

using boost::asio::ip::tcp;

#define INFO_PATH ("me.info")
#define SERVER_PATH ("server.info")
#define INFO_DELIMETER (':')
#define LINE_PREFIX (") ")
#define LINE_SUFFIX ("\n")
#define MENU_PREFIX ("\n")
#define MENU_SUFFIX ("?\n")
#define TMP_FILENAME ("tmp")
#define VER (2)

#define	REGISTER ("Register")
#define	REQ_LST ("Request for clients list")
#define	REQ_PUBKEY ("Request for public key")
#define	REQ_MSGS ("Request for waiting messages")
#define	SEND_MSG ("send a text message")
#define	REQ_SYMKEY ("Send a request for symmetric key")
#define	SEND_SYMKEY ("Send your symmetric key")
#define	SEND_FILE ("Send a file")
#define	EXIT_CLIENT ("Exit client")

#define MYNAME_PROMPT ("please enter your name:")
#define CLIENTID_TITLE ("Client ID: ")
#define CLIENTNAME_TITLE ("Client Name: ")
#define MSG_END ("----------")
#define SENDER_NAME_NOT_AVAILABLE ("sender name is not available")
#define CONTENT_TITLE ("Content:")
#define SYMKEY_REQ_TITLE ("Request for symmetric key")
#define SYMKEY_RECV_TITLE ("Symmetric key received")
#define RECID_PROMPT ("please enter the name of the recepient:")
#define MSGCONF_TITLE(dest) ("your message to " + dest + " was received")
#define SIGNUP_SUCCESS ("signup successful")
#define FILEPATH_PROMPT ("please enter the path of the file to send:")
#define MSG_PROMPT ("please enter your message (feed eof when done):")
#define PUBKEY_RECEIVED_TITLE ("received public key for ")
#define NO_CLIENTS ("there are currently no registered clients")
#define NO_MSGS ("no messages for you")
#define TER_MSG ("terminating program")

#define SERVER_INFO_ERR ("error: couldn't read server info")
#define CONN_ERR ("error: couldn't connect to server")
#define INVALID_NAME_ERR ("error: name not valid - can't be empty")
#define SIGNUP_ERR ("error: you are not allowed to signup twice\n")
#define INVALID_DEST_ERR ("error: invalid recepient")
#define INVALID_PATH_ERR ("error: invalid path")
#define INVALID_OP_ERR ("error: invalid choice")
#define INVALID_TYPE_ERR ("error: got invalid msg type")
#define SERVER_RES_ERR ("server responded with an error")
#define INVALID_RES_ERR ("error: got an invalid response code from server")
#define INVALID_MSGSIZE_ERR ("error: invalid msg size")
#define INVALID_PUBKEY_ERR ("error: received invalid public key")
#define RECV_ERR ("error: received smaller data than expected")
#define SEND_ERR ("error: sent smaller data than expected")
#define NO_PUBKEY_ERR ("error: you don't have the recipient public key")
#define NO_SYMKEY_ERR ("error: you havn't agreed with the recipient on a symetric key")
#define INFO_ERR ("error: your registration file is corrupt")
#define DCRPT_ERR ("can't decrypt message")
#define CLIENT_RUN_ERR ("An error has occurred. Please check your registration file as it might be corrupt.")

#define SIGNUP_OP (10)
#define REQ_LST_OP (20)
#define REQ_PUBKEY_OP (30)
#define REQ_RECV_MSGS_OP (40)
#define SEND_MSG_OP (50)
#define REQ_SYMKEY_OP (51)
#define SEND_SYMKEY_OP (52)
#define SEND_FILE_OP (53)
#define EXIT_OP (0)

#define SIGNUP_REQ (1000)
#define LST_REQ (1001)
#define PUBKEY_REQ (1002)
#define SEND_MSG_REQ (1003)
#define GET_MSGS_REQ (1004)

#define SIGNUP_RES (2000)
#define LST_RES (2001)
#define PUBKEY_RES (2002)
#define MSG_CONF_RES (2003)
#define INCOMING_MSGS_RES (2004)
#define ERR_RES (9000)

#define REQ_SYMKEY_TYPE (1)
#define RES_SYMKEY_TYPE (2)
#define TEXT_TYPE (3)
#define FILE_TYPE (4)

#define MSGID_SIZE (4)
#define MSGTYPE_SIZE (1)
#define VER_SIZE (1)
#define CLIENTID_SIZE (16)
#define CLIENTNAME_SIZE (255)
#define PUBKEY_SIZE (RSAPublicWrapper::KEYSIZE)
#define SYMKEY_SIZE (AESWrapper::DEFAULT_KEYLENGTH)
#define OP_SIZE (2)
#define LOAD_SIZE (4)
#define REQHEADER_SIZE (CLIENTID_SIZE + VER_SIZE + OP_SIZE + LOAD_SIZE)
#define RESHEADER_SIZE (VER_SIZE + OP_SIZE + LOAD_SIZE)
#define PUBKEYREQ_SIZE (CLIENTID_SIZE)
#define MSGCONF_SIZE (CLIENTID_SIZE + MSGID_SIZE)
#define RES_MSGHEADER_SIZE (CLIENTID_SIZE + MSGID_SIZE + MSGTYPE_SIZE + LOAD_SIZE)
#define REQ_MSGHEADER_SIZE (CLIENTID_SIZE + MSGTYPE_SIZE + LOAD_SIZE)
#define BLOB_SIZE (1024)
#define USER_SIZE (CLIENTID_SIZE + CLIENTNAME_SIZE)
#define USERIO_SIZE (USER_SIZE * (BLOB_SIZE / USER_SIZE))

#define RES_VER_POS (0)
#define RES_OP_POS (RES_VER_POS + VER_SIZE)
#define RES_LOADSIZE_POS (RES_OP_POS + OP_SIZE)
#define RES_PAYLOAD_POS (RESHEADER_SIZE)
#define RES_CLIENTID_POS (RES_PAYLOAD_POS)
#define RES_PUBKEY_POS (RES_PAYLOAD_POS + CLIENTID_SIZE)
#define RES_MSGID_POS (RES_PAYLOAD_POS + CLIENTID_SIZE)
#define RES_MSGTYPE_POS (RES_MSGID_POS + MSGID_SIZE)
#define RES_MSGSIZE_POS (RES_MSGTYPE_POS + MSGTYPE_SIZE)
#define RES_MSGLOAD_POS (RES_MSGSIZE_POS + LOAD_SIZE)

#define REQ_CLIENTID_POS (0)
#define REQ_VER_POS (REQ_CLIENTID_POS + CLIENTID_SIZE)
#define REQ_OP_POS (REQ_VER_POS+ VER_SIZE)
#define REQ_LOADSIZE_POS (REQ_OP_POS + OP_SIZE)
#define REQ_PAYLOAD_POS (REQ_LOADSIZE_POS + LOAD_SIZE)
#define REQ_RECID_POS (REQHEADER_SIZE)
#define REQ_MYNAME_POS (REQHEADER_SIZE)
#define REQ_PUBKEY_POS (REQ_MYNAME_POS + CLIENTNAME_SIZE)
#define REQ_MSGTYPE_POS (REQ_RECID_POS + CLIENTID_SIZE)
#define REQ_MSGSIZE_POS (REQ_MSGTYPE_POS + MSGTYPE_SIZE)
#define REQ_MSGLOAD_POS (REQ_MSGSIZE_POS + LOAD_SIZE)

typedef std::string str_t;
typedef uint8_t msgtype_t;
typedef uint8_t ver_t;
typedef uint16_t op_t;
typedef uint32_t loadsize_t;
typedef uint32_t msgsize_t;
typedef std::string clientid_t;
typedef std::array<char, CLIENTNAME_SIZE> clientname_t;
typedef std::array<char, MSGID_SIZE> msgid_t;
typedef std::array<char, OP_SIZE> opbytes_t;
typedef std::array<char, LOAD_SIZE> sizebytes_t;
typedef std::array<char, BLOB_SIZE> blob_t;
typedef str_t pubkey_t;
typedef str_t symkey_t;
typedef unsigned int len_t;
typedef std::vector<char> vec_t;
typedef RSAPublicWrapper rsapub_t;
typedef RSAPrivateWrapper rsapriv_t;
typedef AESWrapper aes_t;
typedef boost::asio::ip::tcp::socket sock_t;

class utils {

public:
	static const str_t get_cur_path();
	static const str_t get_tmp_path();
	static const str_t get_tmpfile_path();
	static const str_t get_rel_path(const str_t& filename);
	static const str_t get_info_path();
	static const str_t get_server_path();
	static const str_t get_input_path();
	static const op_t get_input_op();
	static clientname_t get_myname();
	static clientid_t* get_myid();
	static const str_t get_mykey();
	static const str_t get_server_info();
	static const int get_infodel_pos(const str_t& info);
	static const str_t get_address(const str_t& info, const int pos);
	static const str_t get_port(const str_t& info, const int pos);

	static const bool get_is_little_end();
	static const sizebytes_t get_littlend_loadsize(const loadsize_t);
	static const opbytes_t get_littlend_op(const op_t);
	static const loadsize_t get_loadsize_val(const sizebytes_t loadsize_arr);
	static const op_t get_op_val(const opbytes_t op_arr);
	static clientid_t get_clientid(char* start);

	static const bool is_registered();
	static const bool is_to_user(const op_t);
	static const bool is_to_server(const op_t);
	static const bool is_use_symkey(op_t op);
	static const bool is_use_pubkey(op_t op);

	static const str_t align_op(const unsigned int op);
	static void init_cin();
	static const clientname_t write_client_name();
	static void write_line_info(const str_t& line);

	static void display_menu();
	static void display_signup_res();
	static void display_filepath(const str_t& filename);
	static void display_clientname(const str_t& clientname);
	static void display_msgeof();
	static void display_msg_source(const str_t& clientname);
	static void display_clientname_not_available();
	static void display_content_title();
	static void display_symkey_req_title();
	static void display_symkey_recv_title();
	static void display_prompt_for_recipientid();
	static void display_prompt_for_msg();
	static void display_msgconf(const str_t& clientname);
	static void display_server_err();
	static void display_decrypt_err();
	static void display_client_run_err();
};

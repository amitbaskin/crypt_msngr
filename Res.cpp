#include "Res.h"
#include <iostream>

Res::Res(const vec_t& vec) : vec(vec) {}

const op_t Res::get_op() {
	opbytes_t result;
	char* start = vec.data() + VER_SIZE;
	char* end = start + OP_SIZE;
	char* target = result.data();
	std::copy(start, end, result.data());
	return utils::get_op_val(result);
}

const loadsize_t Res::get_loadsize() {
	sizebytes_t loadsize_bytes;
	char* start = vec.data() + VER_SIZE + OP_SIZE;
	char* end = start + LOAD_SIZE;
	char* target = loadsize_bytes.data();
	std::copy(start, end, target);
	return utils::get_loadsize_val(loadsize_bytes);
}

const msgtype_t Res::get_msgtype() {
	msgtype_t msgtype;
	char* start = vec.data() + RES_MSGTYPE_POS;
	char* end = start + MSGTYPE_SIZE;
	char* target = (char*)&msgtype;
	std::copy(start, end, target);
	return msgtype;
}

const loadsize_t Res::get_msgsize() {
	sizebytes_t msgsize_bytes;
	char* start = vec.data() + RES_MSGSIZE_POS;
	char* end = start + LOAD_SIZE;
	char* target = msgsize_bytes.data();
	std::copy(start, end, target);
	return utils::get_loadsize_val(msgsize_bytes);
}

const clientid_t Res::get_clientid() {
	clientid_t clientid(CLIENTID_SIZE, '\0');
	char* start = vec.data() + RES_CLIENTID_POS;
	char* end = start + CLIENTID_SIZE;
	char* target = clientid.data();
	std::copy(start, end, target);
	return clientid;
}

const pubkey_t Res::get_pubkey() {
	pubkey_t pubkey(PUBKEY_SIZE, '\0');
	char* start = vec.data() + RES_PUBKEY_POS;
	char* end = start + PUBKEY_SIZE;
	char* target = pubkey.data();
	std::copy(start, end, target);
	return pubkey;
}

const msgid_t Res::get_msgid() {
	msgid_t msgid;
	char* start = vec.data() + RES_MSGID_POS;
	char* end = start + MSGID_SIZE;
	char* target = msgid.data();
	std::copy(start, end, target);
	return msgid;
}

const str_t Res::get_msgid_str() {
	str_t msgid_s(MSGID_SIZE, '\0');
	char* start = (char*)get_msgid().data();
	char* end = start + MSGID_SIZE;
	char* target = msgid_s.data();
	std::copy(start, end, target);
	return msgid_s;
}

const vec_t Res::get_payload(const size_t offset, const size_t amount) {
	vec_t result;
	result.resize(amount);
	char* start = vec.data() + RES_MSGLOAD_POS + offset;
	char* end = start + amount;
	char* target = result.data();
	std::copy(start, end, target);
	return result;
}

void Res::insert_msg_header(const vec_t& data) {
	vec.insert(vec.begin() + RES_CLIENTID_POS, data.begin(), data.end());
}

void Res::insert_payload(const vec_t& data) {
	vec.insert(vec.begin() + RES_PAYLOAD_POS, data.begin(), data.end());
}

void Res::insert_msg_payload(const vec_t& data) {
	vec.insert(vec.begin() + RES_MSGLOAD_POS, data.begin(), data.end());
}

#include "Req.h"

Req::Req(const clientid_t& myid) {
	ver_t ver = VER;
	vec.insert(vec.begin(), myid.begin(), myid.end());
	vec.insert(vec.begin() + CLIENTID_SIZE, &ver, &ver + VER_SIZE);
}

void Req::set_signup_req(const pubkey_t& pubkey) {
	set_op(SIGNUP_REQ);
	loadsize_t loadsize = CLIENTNAME_SIZE + pubkey.size();
	set_size(loadsize);	
	clientname_t myname = utils::write_client_name();	
	vec.insert(vec.begin() + REQ_MYNAME_POS, myname.begin(), myname.end());
	vec.insert(vec.begin() + REQ_PUBKEY_POS, pubkey.begin(), pubkey.end());	
}

void Req::set_pubkey_req(const clientid_t& destid) {	
	set_req_header(PUBKEY_REQ, CLIENTID_SIZE);
	vec.insert(vec.begin() + REQ_RECID_POS, destid.begin(), destid.end());
}

void Req::set_symkey_req(const clientid_t& destid) {
	set_req_header(SEND_MSG_REQ, REQ_MSGHEADER_SIZE);
	set_msg_header(destid, REQ_SYMKEY_TYPE, 0);
}

void Req::set_send_symkey(const clientid_t& destid, const symkey_t& symkey) {
	set_req_header(SEND_MSG_REQ, REQ_MSGHEADER_SIZE + symkey.size());
	set_msg_header(destid, RES_SYMKEY_TYPE, symkey.size());
	vec.insert(vec.begin() + REQ_MSGLOAD_POS, symkey.begin(), symkey.end());
}

void Req::set_send_msg(const clientid_t& destid, const msgtype_t msgtype, const loadsize_t loadsize) {
	set_req_header(SEND_MSG_REQ, REQ_MSGHEADER_SIZE + loadsize);
	set_msg_header(destid, msgtype, loadsize);
}

void Req::set_msg_header(const clientid_t& destid, const msgtype_t msgtype, const loadsize_t loadsize) {	
	vec.insert(vec.begin() + REQ_RECID_POS, destid.begin(), destid.end());
	vec.insert(vec.begin() + REQ_MSGTYPE_POS, &msgtype, &msgtype + MSGTYPE_SIZE);
	sizebytes_t sizebytes = utils::get_littlend_loadsize(loadsize);
	vec.insert(vec.begin() + REQ_MSGSIZE_POS, sizebytes.begin(), sizebytes.end());	
}

void Req::set_req_header(const op_t op, const loadsize_t loadsize) {
	set_op(op);
	set_size(loadsize);
}

void Req::set_op(const op_t op) {
	opbytes_t opbytes = utils::get_littlend_op(op);	
	vec.insert(vec.begin() + REQ_OP_POS, opbytes.begin(), opbytes.end());
}

void Req::set_size(const loadsize_t loadsize) {
	sizebytes_t sizebytes = utils::get_littlend_loadsize(loadsize);	
	vec.insert(vec.begin() + REQ_LOADSIZE_POS, sizebytes.begin(), sizebytes.end());
}

const sizebytes_t Req::get_loadsize() {
	sizebytes_t sizebytes;
	char* start = vec.data() + REQ_LOADSIZE_POS;
	char* end = start + LOAD_SIZE;
	char* target = sizebytes.data();
	std::copy(start, end, target);
	return sizebytes;
}

const vec_t Req::get_vec() const {
	return vec;
}

const loadsize_t Req::get_vecsize() const {
	return vec.size();
}

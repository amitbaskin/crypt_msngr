#pragma once

#include "utils.h"

class Req {
private:
	vec_t vec;

public:
	Req(const clientid_t& myid);
	void set_signup_req(const pubkey_t&);
	void set_pubkey_req(const clientid_t& destid);
	void set_symkey_req(const clientid_t& destid);
	void set_send_symkey(const clientid_t& destid, const symkey_t&);
	void set_send_msg(const clientid_t& destid, const msgtype_t, const loadsize_t);
	void set_msg_header(const clientid_t& destid, const msgtype_t, const loadsize_t);
	void set_req_header(const op_t, const loadsize_t);
	void set_op(const op_t);
	void set_size(const loadsize_t);
	void set_clientid(const clientid_t&);
	const vec_t get_vec() const;
	const sizebytes_t get_loadsize();
	const loadsize_t get_vecsize() const;
};

#pragma once

#include "utils.h"

class Res
{
private:
	vec_t vec;

public:
	Res(const vec_t& vec);
	const op_t get_op();
	const loadsize_t get_loadsize();
	const clientid_t get_clientid();
	const pubkey_t get_pubkey();
	const msgid_t get_msgid();
	const str_t get_msgid_str();
	const msgtype_t get_msgtype();
	const loadsize_t get_msgsize();
	const vec_t get_payload(const size_t offset, const size_t amount);
	void insert_msg_header(const vec_t&);
	void insert_msg_payload(const vec_t& data);
	void insert_payload(const vec_t& data);
};

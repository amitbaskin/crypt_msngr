#pragma once

#include "utils.h"
#include "Keys.h"
#include "Req.h"

class Sender
{
private:
	sock_t* sock = nullptr;
	rsapub_t* rsapub = nullptr;
	Keys* keys = nullptr;
	clientid_t* myid = nullptr;

	Sender(const Sender&) = delete;
	Sender(Sender&&) = delete;
	Sender& operator=(const Sender&) = delete;
	Sender& operator=(Sender&&) = delete;

	const op_t send_server(const op_t) const;
	const op_t send_user(const op_t) const;
	void send_signup() const;
	void send_req_lst() const;
	void send_req_pubkey(const clientid_t&) const;
	void send_req_symkey(const clientid_t&) const;
	void send_symkey(const clientid_t&, rsapub_t*) const;
	void send_req_msgs() const;
	void send_msg(const clientid_t&, const msgtype_t) const;
	void send_file(const str_t& filepath, const clientid_t&) const;
	void send_str(const str_t& s) const;
	void send_req(const Req&) const;
	const size_t send(const vec_t&, const size_t offset, const size_t amount) const;
	const size_t send(const str_t&, const size_t offset, const size_t len) const;
	const op_t use_pubkey(const clientid_t&) const;
	const op_t use_symkey(const clientid_t&, const op_t) const;
	const str_t load_stream(const std::istream&, const clientid_t&) const;
	const clientid_t get_input_recepient() const;

public:
	Sender(clientid_t* myid, sock_t*, Keys*, rsapub_t*);
	virtual ~Sender();
	const op_t run(const op_t) const;
};

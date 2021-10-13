#pragma once

#include "utils.h"
#include "Keys.h"
#include "Res.h"

class Receiver
{
private:
	sock_t* sock = nullptr;
	rsapriv_t* rsapriv = nullptr;
	Keys* keys = nullptr;
	clientid_t* myid = nullptr;

	Receiver(const Receiver&) = delete;
	Receiver(Receiver&&) = delete;
	Receiver& operator=(const Receiver&) = delete;
	Receiver& operator=(Receiver&&) = delete;

	void recv_signup_conf(Res&);
	void recv_confmsg(Res&) const;
	void recv_users_lst(Res& res);
	void recv_pubkey(Res&);
	void recv_symkey(Res&);
	void recv_all_msgs(Res&);
	const int recv_user_msg(Res&);
	void recv_file(Res&, aes_t*) const;
	const str_t recv_stream(const loadsize_t size, aes_t* aes) const;
	const vec_t recv(const size_t) const;
	const size_t recv(vec_t&, const size_t) const;
	const size_t recv(str_t&, size_t offset, const size_t amount) const;
	void write_str(const str_t&, std::ostream& file) const;
	void display_users(vec_t&, int users_amount);
	void display_msg_prefix(Res&);
	void display_pubkey_received(const clientid_t&) const;

public:
	Receiver(clientid_t* myid, sock_t*, Keys*, rsapriv_t*);
	virtual ~Receiver();
	const op_t recv_res();
};
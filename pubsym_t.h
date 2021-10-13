#include "utils.h"

#pragma once
class pubsym_t
{
private:
	rsapub_t* rsapub = nullptr;
	aes_t* aes = nullptr;
	str_t clientname;

	pubsym_t(const pubsym_t&) = delete;
	pubsym_t(pubsym_t&&) = delete;
	pubsym_t& operator=(const pubsym_t&) = delete;
	pubsym_t& operator=(pubsym_t&&) = delete;

public:
	pubsym_t();
	virtual ~pubsym_t();
	rsapub_t* set_pubkey(const pubkey_t&);
	aes_t* set_symkey(const symkey_t&);
	aes_t* set_new_symkey();
	void set_clientname(const str_t& clientname);
	const str_t get_clientname() const;
	rsapub_t* get_rsapub() const;
	aes_t* get_aes() const;
};

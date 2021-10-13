#include "utils.h"
#include "pubsym_t.h"
#include <unordered_map>

typedef std::unordered_map<clientid_t, pubsym_t*> keys_t;

#pragma once
class Keys
{
private:
	keys_t* keys = nullptr;
	Keys(const Keys&) = delete;
	Keys(Keys&&) = delete;
	Keys& operator=(const Keys&) = delete;
	Keys& operator=(Keys&&) = delete;

public:
	Keys();
	virtual ~Keys();
	pubsym_t* put_pubsym(const clientid_t&);
	rsapub_t* put_pubkey(const clientid_t&, const pubkey_t&);
	aes_t* put_symkey(const clientid_t&, const symkey_t&);
	aes_t* put_new_sym(const clientid_t&);
	void put_clientname(const clientid_t&, const str_t& clientname);
	str_t get_clientname(const clientid_t&) const;
	pubsym_t* get_pubsym(const clientid_t&) const;
	aes_t* get_aes(const clientid_t&) const;
	rsapub_t* get_rsapub(const clientid_t&) const;
	const str_t aes_enc(const clientid_t&, const str_t&) const;
	const str_t aes_dec(const clientid_t&, const str_t&) const;
	const str_t rsapub_enc(const clientid_t&, const str_t&) const;
	const clientid_t get_clientid(const str_t& clientname) const;
};

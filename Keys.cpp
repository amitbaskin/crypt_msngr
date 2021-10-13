#include "Keys.h"
#include "aes.h"
#include "pubsym_t.h"
#include <cstring>

Keys::Keys() {
	keys = new keys_t();
}

Keys::~Keys() {
	for (std::pair<const clientid_t, pubsym_t*> pair : *keys) {
		delete pair.second;
	}
	delete keys;
}

pubsym_t* Keys::put_pubsym(const clientid_t& clientid) {
	pubsym_t* x = get_pubsym(clientid);
	if (x != nullptr) return x;
	keys->insert({ clientid, x = new pubsym_t() });
	return x;
}

pubsym_t* Keys::get_pubsym(const clientid_t& clientid) const {
	keys_t::const_iterator iter = keys->find(clientid);
	if (iter == keys->end()) return nullptr;
	return iter->second;
}

rsapub_t* Keys::get_rsapub(const clientid_t& clientid) const {
	pubsym_t* x = get_pubsym(clientid);
	if (x == nullptr) return nullptr;
	return x->get_rsapub();
}

rsapub_t* Keys::put_pubkey(const clientid_t& clientid, const pubkey_t& pubkey) {
	pubsym_t* x = get_pubsym(clientid);
	if (x == nullptr) {
		x = put_pubsym(clientid);
	}
	return x->set_pubkey(pubkey);
}

aes_t* Keys::get_aes(const clientid_t& clientid) const {
	pubsym_t* x = get_pubsym(clientid);
	if (x == nullptr) return nullptr;
	return x->get_aes();
}

aes_t* Keys::put_symkey(const clientid_t& clientid, const symkey_t& sym) {
	pubsym_t* x = get_pubsym(clientid);
	if (x == nullptr) {
		x = put_pubsym(clientid);
	}
	return x->set_symkey(sym);
}

aes_t* Keys::put_new_sym(const clientid_t& clientid) {
	pubsym_t* x = get_pubsym(clientid);
	if (x == nullptr) {
		x = put_pubsym(clientid);
	}
	return x->set_new_symkey();
}

str_t Keys::get_clientname(const clientid_t& clientid) const {
	keys_t::const_iterator iter = keys->find(clientid);
	if (iter == keys->end()) return str_t("");
	return iter->second->get_clientname();
}

void Keys::put_clientname(const clientid_t& clientid, const str_t& clientname) {
	pubsym_t* x = get_pubsym(clientid);
	if (x == nullptr) {
		x = put_pubsym(clientid);
	}
	return x->set_clientname(clientname);
}


const clientid_t Keys::get_clientid(const str_t& clientname) const {
	for (const auto& [key, value] : *keys) {
		if (std::strcmp(value->get_clientname().data(), clientname.data()) == 0) return key;
	}
	throw std::exception(INVALID_DEST_ERR);
}

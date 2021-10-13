#include "pubsym_t.h"
#include "aes.h"

pubsym_t::pubsym_t() {}

pubsym_t::~pubsym_t() {
	if (rsapub != nullptr) delete rsapub;
	if (aes != nullptr) delete aes;
}

rsapub_t* pubsym_t::set_pubkey(const pubkey_t& pubkey) {
	if (rsapub == nullptr) {
		try {
			rsapub = new RSAPublicWrapper(pubkey.data(), pubkey.size());
		}
		catch (std::exception&) {
			throw std::exception(INVALID_PUBKEY_ERR);
		}
	}
	return rsapub;
}

aes_t* pubsym_t::set_symkey(const symkey_t& symkey) {
	aes = new aes_t((unsigned char*)symkey.data(), symkey.size());
	return aes;
}

rsapub_t* pubsym_t::get_rsapub() const {
	return rsapub;
}

aes_t* pubsym_t::get_aes() const {
	return aes;
}

aes_t* pubsym_t::set_new_symkey() {
	aes = new aes_t();
	return aes;
}

void pubsym_t::set_clientname(const str_t& clientname) {
	this->clientname = clientname;
}

const str_t pubsym_t::get_clientname() const {
	return clientname;
}

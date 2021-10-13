#include <fstream>
#include <filesystem>
#include "Sender.h"
#include "Req.h"
#include "rsa.h"
#include "aes.h"
#include "Base64Wrapper.h"

Sender::Sender(clientid_t* myid, sock_t* sock, Keys* keys, rsapub_t* rsapub) {
	this->myid = myid;
	this->sock = sock;
	this->keys = keys;
	this->rsapub = rsapub;
}

Sender::~Sender() {
	delete rsapub;
	rsapub = nullptr;
}

const op_t Sender::run(const op_t op) const {
	if (utils::is_to_user(op)) return send_user(op);
	else if (utils::is_to_server(op)) return send_server(op);
	else throw std::exception(INVALID_OP_ERR);
}

const op_t Sender::send_server(const op_t op) const {
	switch (op) {
	case SIGNUP_OP:
		send_signup();
		return SIGNUP_RES;
	case REQ_LST_OP:
		send_req_lst();
		return LST_RES;
	case REQ_RECV_MSGS_OP:
		send_req_msgs();
		return INCOMING_MSGS_RES;
	default:
		throw std::exception(INVALID_OP_ERR);
	}
}

const op_t Sender::send_user(const op_t op) const {
	clientid_t clientid = get_input_recepient();
	if (utils::is_use_symkey(op)) return use_symkey(clientid, op);
	if (utils::is_use_pubkey(op)) return use_pubkey(clientid);
	switch (op) {
	case REQ_PUBKEY_OP:
		send_req_pubkey(clientid);
		return PUBKEY_RES;
	case REQ_SYMKEY_OP:
		send_req_symkey(clientid);
		return INCOMING_MSGS_RES;
	default:
		throw std::exception(INVALID_OP_ERR);
	}
}

void Sender::send_signup() const {
	Req req = Req(*myid);
	pubkey_t pubkey = rsapub->getPublicKey();
	req.set_signup_req(pubkey);
	send_req(req);
}

void Sender::send_req_lst() const {
	Req req = Req(*myid);
	req.set_req_header(LST_REQ, 0);
	send_req(req);
}

void Sender::send_req_pubkey(const clientid_t& clientid) const {
	Req req = Req(*myid);
	req.set_pubkey_req(clientid);
	send_req(req);
}

void Sender::send_req_symkey(const clientid_t& clientid) const {
	Req req = Req(*myid);
	req.set_symkey_req(clientid);
	send_req(req);
}

void Sender::send_req_msgs() const {
	Req req = Req(*myid);
	req.set_req_header(GET_MSGS_REQ, 0);
	send_req(req);
}

void Sender::send_symkey(const clientid_t& clientid, rsapub_t* rec_rsapub) const {
	Req req = Req(*myid);
	aes_t* aes = keys->get_aes(clientid);
	if (aes == nullptr) aes = keys->put_new_sym(clientid);
	str_t symkey((char*)aes->getKey(), SYMKEY_SIZE);
	str_t symcipher = rec_rsapub->encrypt(symkey);
	req.set_send_symkey(clientid, symcipher);
	send_req(req);
}

void Sender::send_msg(const clientid_t& clientid, const msgtype_t type) const {
	Req req = Req(*myid);
	utils::display_prompt_for_msg();
	str_t str = load_stream(std::cin, clientid);
	req.set_send_msg(clientid, type, str.size());
	send_req(req);
	send_str(str);
}

void Sender::send_file(const str_t& filepath, const clientid_t& clientid) const {
	Req req = Req(*myid);
	std::ifstream file(filepath, std::ios::binary);
	str_t str;
	try {
		str = load_stream(file, clientid);
	}
	catch (std::exception& e) {
		file.close();
		throw e;
	}
	file.close();
	req.set_send_msg(clientid, FILE_TYPE, str.size());
	send_req(req);
	send_str(str);
}

void Sender::send_req(const Req& req) const {
	loadsize_t size = req.get_vecsize();
	size_t done = 0;
	vec_t vec = req.get_vec();
	if (size - done >= BLOB_SIZE) {
		done += send(vec, done, BLOB_SIZE);
	}
	else {
		done += send(vec, done, size - done);
	}
}

void Sender::send_str(const str_t& s) const {
	size_t counter = 0;
	size_t total = s.size();
	while (counter < total) {
		if (total - counter >= BLOB_SIZE) {
			send(s, counter, BLOB_SIZE);
			counter += BLOB_SIZE;
		}
		else {
			send(s, counter, total - counter);
			counter = total;
		}
	}
}

const size_t Sender::send(const vec_t& vec, const size_t offset, const size_t amount) const {
	size_t sent = boost::asio::write(*sock, boost::asio::buffer(vec.data() + offset, amount));
	if (sent < vec.size()) throw std::exception(SEND_ERR);
	return sent;
}

const size_t Sender::send(const str_t& str, const size_t offset, const size_t len) const {
	size_t sent = boost::asio::write(*sock, boost::asio::buffer(str.data() + offset, len));
	if (sent < len) throw std::exception(SEND_ERR);
	return sent;
}

const op_t Sender::use_pubkey(const clientid_t& clientid) const {
	rsapub_t* rec_rsapub;
	if ((rec_rsapub = keys->get_rsapub(clientid)) == nullptr) throw std::exception(NO_PUBKEY_ERR);
	send_symkey(clientid, rec_rsapub);
	return MSG_CONF_RES;
}

const op_t Sender::use_symkey(const clientid_t& clientid, const op_t op) const {
	switch (op) {
	case SEND_MSG_OP:
		send_msg(clientid, TEXT_TYPE);
		return MSG_CONF_RES;
	case SEND_FILE_OP:
		send_file(utils::get_input_path(), clientid);
		return MSG_CONF_RES;
	default:
		throw std::exception(INVALID_OP_ERR);
	}
}

const str_t Sender::load_stream(const std::istream& stream, const clientid_t& clientid) const {
	aes_t* aes = keys->get_aes(clientid);
	if (aes == nullptr) throw std::exception(NO_SYMKEY_ERR);
	std::stringstream s_stream;
	s_stream << stream.rdbuf();
	str_t s = s_stream.str();
	return aes->encrypt(s.data(), s.size());
}

const clientid_t Sender::get_input_recepient() const {
	str_t clientname(CLIENTNAME_SIZE, '\0');
	utils::display_prompt_for_recipientid();
	utils::init_cin();
	std::cin.getline(clientname.data(), CLIENTNAME_SIZE);
	return keys->get_clientid(clientname);
}

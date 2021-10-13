#include <fstream>
#include <filesystem>
#include <sstream>
#include <boost/algorithm/hex.hpp>
#include "Receiver.h"
#include "rsa.h"
#include "Base64Wrapper.h"

Receiver::Receiver(clientid_t* myid, sock_t* sock, Keys* keys, rsapriv_t* rsapriv) {
	this->myid = myid;
	this->sock = sock;
	this->keys = keys;
	this->rsapriv = rsapriv;
}

Receiver::~Receiver() {
	delete rsapriv;
	rsapriv = nullptr;
}

void Receiver::recv_all_msgs(Res& res) {
	loadsize_t bytes_read = 0;
	loadsize_t bytes_to_read = res.get_loadsize();
	if (bytes_to_read == 0) std::cout << NO_MSGS << std::endl;
	while (bytes_read < bytes_to_read) {
		recv_user_msg(res);
		bytes_read += RES_MSGHEADER_SIZE;
		bytes_read += res.get_msgsize();
	}
}

const int Receiver::recv_user_msg(Res& res) {
	res.insert_msg_header(recv(RES_MSGHEADER_SIZE));
	display_msg_prefix(res);
	msgtype_t msgtype = res.get_msgtype();
	switch (msgtype) {
	case REQ_SYMKEY_TYPE:
		utils::display_symkey_req_title();
		utils::display_msgeof();
		return RES_SYMKEY_TYPE;
	case RES_SYMKEY_TYPE:
		recv_symkey(res);
		return RES_SYMKEY_TYPE;
	case TEXT_TYPE:
	case FILE_TYPE:
	{
		aes_t* aes = keys->get_aes(res.get_clientid());
		if (aes == nullptr) throw std::exception(DCRPT_ERR);
		if (msgtype == TEXT_TYPE) {
			write_str(recv_stream(res.get_msgsize(), aes), std::cout);
			utils::display_msgeof();
			return TEXT_TYPE;
		}
		recv_file(res, aes);
		utils::display_msgeof();
		return FILE_TYPE;
	}
	default:
		throw std::exception(INVALID_TYPE_ERR);
	}
}

const op_t Receiver::recv_res() {
	Res res = Res(recv(RESHEADER_SIZE));
	switch (res.get_op()) {
	case SIGNUP_RES:
		recv_signup_conf(res);
		return SIGNUP_RES;
	case LST_RES:
		recv_users_lst(res);
		return SIGNUP_RES;
	case PUBKEY_RES:
		recv_pubkey(res);
		return SIGNUP_RES;
	case MSG_CONF_RES:
		recv_confmsg(res);
		return SIGNUP_RES;
	case INCOMING_MSGS_RES:
		recv_all_msgs(res);
		return INCOMING_MSGS_RES;
	case ERR_RES:
		throw std::exception(SERVER_RES_ERR);
	default:
		throw std::exception(INVALID_RES_ERR);
	}
}

void Receiver::recv_signup_conf(Res& res) {
	vec_t clientid = recv(CLIENTID_SIZE);	
	char* start = clientid.data();
	char* end = start + CLIENTID_SIZE;
	char* target = myid->data();
	std::copy(start, end, target);	
	utils::display_signup_res();	
	utils::write_line_info(boost::algorithm::hex(*myid));
	utils::write_line_info(Base64Wrapper::encode(rsapriv->getPrivateKey()));
}

void Receiver::recv_users_lst(Res& res) {
	vec_t vec;
	vec.resize(USERIO_SIZE);
	unsigned bytes_read = 0;
	size_t size = res.get_loadsize();
	if (size == 0) std::cout << NO_CLIENTS << std::endl;
	while (bytes_read < size) {
		if (size - bytes_read >= USERIO_SIZE) {
			bytes_read += recv(vec, USERIO_SIZE);
			display_users(vec, BLOB_SIZE / USER_SIZE);
		}
		else {
			size_t tmp = (size - bytes_read) / USER_SIZE;
			bytes_read += recv(vec, size - bytes_read);
			display_users(vec, tmp);
		}
	}
}
void Receiver::recv_pubkey(Res& res) {
	res.insert_payload(recv(CLIENTID_SIZE + PUBKEY_SIZE));
	clientid_t clientid = res.get_clientid();
	keys->put_pubkey(clientid, res.get_pubkey());
	display_pubkey_received(clientid);
}

void Receiver::recv_symkey(Res& res) {
	loadsize_t loadsize = res.get_msgsize();
	res.insert_msg_payload(recv(loadsize));
	vec_t vec = res.get_payload(0, loadsize);
	try {
		keys->put_symkey(res.get_clientid(), rsapriv->decrypt(vec.data(), vec.size()));
	}
	catch (std::exception&) {
		throw std::exception(DCRPT_ERR);
	}	
	utils::display_symkey_recv_title();
	utils::display_msgeof();
}

void Receiver::recv_confmsg(Res& res) const {
	res.insert_payload(recv(MSGCONF_SIZE));
	utils::display_msgconf(keys->get_clientname(res.get_clientid()));
}

void Receiver::recv_file(Res& res, aes_t* aes) const {
	const str_t filepath = utils::get_rel_path(res.get_msgid_str());
	utils::display_filepath(filepath);
	std::ofstream file(filepath, std::ios::binary);
	try {
		write_str(recv_stream(res.get_msgsize(), aes), file);
	}
	catch (std::exception& e) {
		file.close();
		throw e;
	}
	file.close();
}

const str_t Receiver::recv_stream(const loadsize_t size, aes_t* aes) const {
	size_t counter = 0;
	str_t s(size, '\0');
	while (counter < size) {
		if (size - counter >= BLOB_SIZE) {
			recv(s, counter, BLOB_SIZE);
			counter += BLOB_SIZE;
		}
		else {
			recv(s, counter, size - counter);
			counter += size;
		}
	}
	try {
		return aes->decrypt(s.data(), size);
	}
	catch (std::exception&) {
		throw std::exception(DCRPT_ERR);
	}
}

const vec_t Receiver::recv(const size_t amount) const {
	vec_t vec;
	vec.resize(amount);
	size_t received = boost::asio::read(*sock, boost::asio::buffer(vec.data(), amount));
	if (received < amount) throw std::exception(RECV_ERR);
	return vec;
}

const size_t Receiver::recv(vec_t& vec, const size_t amount) const {
	size_t received = boost::asio::read(*sock, boost::asio::buffer(vec.data(), amount));
	if (received < amount) throw std::exception(RECV_ERR);
	return received;
}

const size_t Receiver::recv(str_t& s, size_t offset, const size_t amount) const {
	size_t received = boost::asio::read(*sock, boost::asio::buffer(s.data() + offset, amount));
	if (received < amount) throw std::exception(RECV_ERR);
	return received;
}

void Receiver::display_msg_prefix(Res& res) {
	str_t clientname = keys->get_clientname(res.get_clientid());
	if (clientname[0] == '\0') utils::display_clientname_not_available();
	else utils::display_msg_source(clientname);
	utils::display_content_title();
}

void Receiver::display_users(vec_t& vec, int users_amount) {
	clientid_t clientid;
	str_t clientname;
	int offset = 0;
	for (int i = 0; i < users_amount; i++) {
		clientid = utils::get_clientid(vec.data() + offset);
		keys->put_pubsym(clientid);
		offset += CLIENTID_SIZE;
		clientname = str_t(vec.data() + offset);
		keys->put_clientname(clientid, clientname);
		utils::display_clientname(clientname);
		offset += CLIENTNAME_SIZE;
	}
}

void Receiver::display_pubkey_received(const clientid_t& clientid) const {
	std::cout << PUBKEY_RECEIVED_TITLE << keys->get_clientname(clientid) << std::endl;
}

void Receiver::write_str(const str_t& s, std::ostream& file) const {
	file.write(s.data(), s.size());
}

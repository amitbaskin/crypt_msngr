#include "Client.h"
#include "utils.h"
#include <fstream>
#include <filesystem>

boost::asio::io_context io_context;
tcp::resolver resolver(io_context);

Client::Client() {
	sock = new sock_t(io_context);
	keys = new Keys();
	RSAPrivateWrapper* priv;
	if (utils::is_registered()) {
		try {
			myid = utils::get_myid();
			str_t key = utils::get_mykey();
			priv = new RSAPrivateWrapper(key.data(), key.size());
		}
		catch (std::exception&) {
			throw std::exception(CLIENT_RUN_ERR);
		}
	}
	else {
		priv = new RSAPrivateWrapper();
		myid = new clientid_t(CLIENTID_SIZE, '\0');
	}
	receiver = new Receiver(myid, sock, keys, priv);
	sender = new Sender(myid, sock, keys, new RSAPublicWrapper(priv->getPublicKey()));
}

Client::~Client() {
	sock->close();
	delete receiver;
	receiver = nullptr;
	delete sender;
	sender = nullptr;
	delete myid;
	myid = nullptr;
	delete sock;
	sock = nullptr;
	delete keys;
	keys = nullptr;
}

void Client::connect(const str_t& address, const str_t& port) {
	try {
		boost::asio::connect(*sock, resolver.resolve(address, port));
	}
	catch (std::exception&) {
		throw std::exception(CONN_ERR);
	}
}


void Client::main() {
	str_t info;
	int pos;
	str_t address;
	str_t port;
	try {
		info = utils::get_server_info();
		pos = utils::get_infodel_pos(info);
		address = utils::get_address(info, pos);
		port = utils::get_port(info, pos);
	}
	catch (std::exception&) {
		throw std::exception(SERVER_INFO_ERR);
	}	
	op_t desired_res = ERR_RES;
	while (true) {
		try {
			utils::display_menu();
			op_t choice = utils::get_input_op();
			if (choice == EXIT_OP) break;
			try {
				connect(address, port);
			}
			catch (std::exception& e) {
				std::cout << e.what() << std::endl;
				exit(1);
			}
			desired_res = sender->run(choice);
			receiver->recv_res();
			sock->close();
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl << std::endl;
			sock->close();
			if (desired_res == SIGNUP_RES) {
				try {
					std::filesystem::remove(utils::get_info_path());
				}
				catch (std::exception&) {}
			}
		}
	}
}

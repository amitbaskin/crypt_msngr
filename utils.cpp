#include "utils.h"
#include "aes.h"
#include "Base64Wrapper.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <boost/algorithm/hex.hpp>

const int x = 1;
const bool is_little_end = *(char*)&x == 1;

const bool utils::get_is_little_end() {
	return is_little_end;
}

const str_t tmp_path = std::filesystem::temp_directory_path().string();
const str_t tmpfile_path = tmp_path + TMP_FILENAME;
const str_t cur_path = std::filesystem::current_path().string() + (char)std::filesystem::path::preferred_separator;
const str_t info_path = cur_path + INFO_PATH;
const str_t server_path = cur_path + SERVER_PATH;

const str_t utils::get_cur_path() {
	return cur_path;
}

const str_t utils::get_tmp_path() {
	return tmp_path;
}

const str_t utils::get_tmpfile_path() {
	return tmpfile_path;
}

const str_t utils::get_info_path() {
	return info_path;
}

const str_t utils::get_server_path() {
	return server_path;
}

const str_t utils::get_rel_path(const str_t& filename) {
	return utils::get_tmp_path() + filename;
}

void utils::write_line_info(const str_t& line) {
	std::ofstream info(utils::get_info_path(), std::ios_base::app);
	info << line << std::endl;
	info.close();
}

const str_t utils::align_op(const unsigned int op) {
	/*explanation for the 1 and 3:
	1 is for the null terminator.
	additional 1 is for the percentage character,
	and another 1 for the character 'd' - the format specifior character.
	i.e. after the first snprintf we get %<OP_SIZE>d
	where the OP_SIZE is replaced by it's value.
	Therefore after the second snprintf we will get the given op
	with the desired padding on its left.*/
	char buf[OP_SIZE + 1];
	char fmt[OP_SIZE + 3];
	snprintf(fmt, OP_SIZE + 3, "%%%d%c", OP_SIZE, 'd');
	snprintf(buf, OP_SIZE + 1, fmt, op);
	return str_t(buf);
}

const str_t menu = str_t(MENU_PREFIX + \
	utils::align_op(SIGNUP_OP) + LINE_PREFIX + REGISTER + LINE_SUFFIX + \
	utils::align_op(REQ_LST_OP) + LINE_PREFIX + REQ_LST + LINE_SUFFIX + \
	utils::align_op(REQ_PUBKEY_OP) + LINE_PREFIX + REQ_PUBKEY + LINE_SUFFIX + \
	utils::align_op(REQ_RECV_MSGS_OP) + LINE_PREFIX + REQ_MSGS + LINE_SUFFIX + \
	utils::align_op(SEND_MSG_OP) + LINE_PREFIX + SEND_MSG + LINE_SUFFIX + \
	utils::align_op(REQ_SYMKEY_OP) + LINE_PREFIX + REQ_SYMKEY + LINE_SUFFIX + \
	utils::align_op(SEND_SYMKEY_OP) + LINE_PREFIX + SEND_SYMKEY + LINE_SUFFIX + \
	utils::align_op(SEND_FILE_OP) + LINE_PREFIX + SEND_FILE + LINE_SUFFIX + \
	utils::align_op(EXIT_OP) + LINE_PREFIX + EXIT_CLIENT + LINE_SUFFIX + \
	MENU_SUFFIX);


void utils::display_menu() {
	std::cout << menu;
}

void utils::init_cin() {
	std::cin.clear();
}

const bool utils::is_registered() {
	return std::filesystem::exists(get_info_path());
}

const str_t utils::get_server_info() {
	str_t info;
	std::ifstream file(utils::get_server_path());	
	if (!file.good()) throw std::exception(SERVER_INFO_ERR);
	std::getline(file, info);
	file.close();
	return info;
}

const int utils::get_infodel_pos(const str_t& info) {
	unsigned int pos = 0;
	for (; (info.at(pos) != INFO_DELIMETER) && (pos < info.size()); pos++);
	if (pos == info.size()) throw std::exception(SERVER_INFO_ERR);
	return pos;
}

const str_t utils::get_address(const str_t& info, int pos) {
	return info.substr(0, pos);
}

const str_t utils::get_port(const str_t& info, int pos) {
	return info.substr(pos + 1, info.size() - pos - 1);
}

const clientname_t utils::write_client_name() {
	if (std::filesystem::exists(get_info_path())) {
		str_t err = SIGNUP_ERR;
		err += "your current name is: ";
		err += str_t(utils::get_myname().data());
		throw std::exception(err.data());
	}
	str_t str(CLIENTNAME_SIZE, '\0');
	while (str[0] == '\0') {
		std::cout << MYNAME_PROMPT << std::endl;
		init_cin();
		std::cin.getline(str.data(), CLIENTNAME_SIZE);
		if (str[0] == '\0') {
			std::cout << INVALID_NAME_ERR << std::endl;
		}
	}
	write_line_info(str);
	clientname_t clientname;
	clientname.fill('\0');
	char* start = str.data();
	char* end = start + str.size();
	char* target = clientname.data();
	std::copy(start, end, target);
	return clientname;
}

clientid_t* utils::get_myid() {
	std::ifstream info(utils::get_info_path());	
	str_t line;
	std::getline(info, line);
	if (info.peek() == EOF) throw std::exception(INFO_ERR);
	std::getline(info, line);
	info.close();
	clientid_t* id = new clientid_t(boost::algorithm::unhex(line));
	if (id->size() != CLIENTID_SIZE) throw std::exception(INFO_ERR);
	return id;
}

clientname_t utils::get_myname() {
	std::ifstream info(utils::get_info_path());
	str_t clientname_s;
	std::getline(info, clientname_s);
	info.close();
	clientname_t clientname;
	clientname.fill('\0');
	char* start = clientname_s.data();
	char* end = start + clientname_s.length();
	char* target = clientname.data();
	std::copy(start, end, target);
	return clientname;
}

const str_t utils::get_mykey() {
	std::ifstream info(utils::get_info_path());
	str_t tmp;
	std::getline(info, tmp);
	if (info.peek() == EOF) if (info.peek() == EOF) throw std::exception(INFO_ERR);
	std::getline(info, tmp);
	if (info.peek() == EOF) if (info.peek() == EOF) throw std::exception(INFO_ERR);
	std::ostringstream buf;
	buf << info.rdbuf();
	info.close();
	return Base64Wrapper::decode(buf.str());
}

const str_t utils::get_input_path() {
	std::cout << FILEPATH_PROMPT << std::endl;
	str_t filepath(MAX_PATH, '\0');
	init_cin();
	std::cin.getline(filepath.data(), MAX_PATH);
	if (!std::filesystem::exists(filepath)) throw std::exception(INVALID_PATH_ERR);
	return filepath;
}

const op_t utils::get_input_op() {
	op_t op;
	str_t inputop;
	init_cin();
	std::cin.getline(inputop.data(), 4 * sizeof(op));
	try { op = (op_t)std::stoi(inputop); }
	catch (std::exception&) { throw std::exception(INVALID_OP_ERR); }
	return op;
}

const bool utils::is_to_server(const op_t op) {
	return op == SIGNUP_OP ||
		op == REQ_LST_OP ||
		op == REQ_PUBKEY_OP ||
		op == REQ_RECV_MSGS_OP ||
		op == EXIT_OP;
}

const bool utils::is_to_user(const op_t op) {
	return op == REQ_PUBKEY_OP ||
		op == SEND_MSG_OP ||
		op == SEND_SYMKEY_OP ||
		op == REQ_SYMKEY_OP ||
		op == SEND_SYMKEY_OP ||
		op == SEND_FILE_OP;
}
const sizebytes_t utils::get_littlend_loadsize(const loadsize_t loadsize) {
	sizebytes_t result;
	sizebytes_t tmp;
	std::copy((char*)&loadsize, (char*)&loadsize + LOAD_SIZE, tmp.data());
	if (get_is_little_end()) return tmp;
	for (int i = 0; i < LOAD_SIZE; i++) {
		result[i] = tmp[LOAD_SIZE - 1 - i];
	}
	return result;
}

const opbytes_t utils::get_littlend_op(const op_t op) {
	opbytes_t tmp;
	opbytes_t result;
	std::copy((char*)&op, (char*)&op + OP_SIZE, tmp.data());
	if (get_is_little_end()) return tmp;
	for (int i = 0; i < OP_SIZE; i++) {
		result[i] = tmp[OP_SIZE - 1 - i];
	}
	return result;
}

const loadsize_t utils::get_loadsize_val(const sizebytes_t loadsize_arr) {
	return (unsigned char)(loadsize_arr[0]) |
		(unsigned char)(loadsize_arr[1]) << 8 |
		(unsigned char)(loadsize_arr[2]) << 16 |
		(unsigned char)(loadsize_arr[3]) << 24;
}

const op_t utils::get_op_val(const opbytes_t op_arr) {
	return (unsigned char)(op_arr[0]) |
		(unsigned char)(op_arr[1]) << 8;
}

clientid_t utils::get_clientid(char* start) {
	clientid_t clientid;
	clientid.insert(0, start, CLIENTID_SIZE);	
	return clientid;
}

void utils::display_clientname(const str_t& clientname) {
	std::cout << CLIENTNAME_TITLE;
	std::cout << clientname << std::endl;
}

void utils::display_msgeof() {
	std::cout << MSG_END << std::endl << std::endl;
}

void utils::display_msg_source(const str_t& clientname) {
	std::cout << "From: " << clientname << std::endl;
}

void utils::display_clientname_not_available() {
	std::cout << SENDER_NAME_NOT_AVAILABLE << std::endl;
}

void utils::display_content_title() {
	std::cout << CONTENT_TITLE << std::endl;
}

void utils::display_symkey_req_title() {
	std::cout << SYMKEY_REQ_TITLE << std::endl;
}

void utils::display_decrypt_err() {
	std::cout << DCRPT_ERR << std::endl;
}

void utils::display_symkey_recv_title() {
	std::cout << SYMKEY_RECV_TITLE << std::endl;
}

void utils::display_prompt_for_recipientid() {
	std::cout << RECID_PROMPT << std::endl;
}

void utils::display_prompt_for_msg() {
	std::cout << MSG_PROMPT << std::endl;
}

void utils::display_msgconf(const str_t& clientname) {
	std::cout << MSGCONF_TITLE(clientname) << std::endl;
}

const bool utils::is_use_symkey(op_t op) {
	return (op == SEND_MSG_OP) || (op == SEND_FILE_OP);
}

const bool utils::is_use_pubkey(op_t op) {
	return op == SEND_SYMKEY_OP;
}

void utils::display_server_err() {
	std::cout << ERR_RES << std::endl << std::endl;
}

void utils::display_signup_res() {
	std::cout << SIGNUP_SUCCESS << std::endl;
}

void utils::display_filepath(const str_t& path) {
	std::cout << path << std::endl;
}

void utils::display_client_run_err() {
	std::cout << CLIENT_RUN_ERR << std::endl;
}
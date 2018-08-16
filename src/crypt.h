#ifndef __SCYTHE_CRYPT_H__
#define __SCYTHE_CRYPT_H__

#include <cstddef>

namespace scythe {
	
	//! Simple encryption/decryption function
	void Encrypt(char* buffer_out, const char* buffer_in, size_t length, const char* key);
	
} // namespace scythe

#endif
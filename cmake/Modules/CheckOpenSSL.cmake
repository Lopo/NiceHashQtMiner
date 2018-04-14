set(SOURCE "
#include <string.h>
#include <openssl/evp.h>


int main()
{
	exit(EVP_aes_128_gcm()==NULL
		|| EVP_aes_256_gcm()==NULL
		|| EVP_CTRL_GCM_SET_IV_FIXED==0
		|| EVP_CTRL_GCM_IV_GEN==0
		|| EVP_CTRL_GCM_SET_TAG==0
		|| EVP_CTRL_GCM_GET_TAG==0
		|| EVP_CIPHER_CTX_ctrl(NULL, 0, 0, NULL)==0
		);
}
")
file(WRITE "${CMAKE_BINARY_DIR}/CMakeTmp/CheckOpenSSL.c" "${SOURCE}")
message(STATUS "Checking whether OpenSSL has AES GCM via EVP") #2668
try_compile(OPENSSL_HAVE_EVPGCM_O
	${CMAKE_BINARY_DIR}
	${CMAKE_BINARY_DIR}/CMakeTmp/CheckOpenSSL.c
	LINK_LIBRARIES ${OPENSSL_LIBRARIES}
	OUTPUT_VARIABLE OUTPUT
	)
if (OPENSSL_HAVE_EVPGCM_O)
	message(STATUS "Checking whether OpenSSL has AES GCM via EVP -- yes")
	set(OPENSSL_HAVE_EVPGCM TRUE)
else ()
	message(STATUS "Checking whether OpenSSL has AES GCM via EVP -- no")
	file(APPEND ${CMAKE_BINARY_DIR}/CMakeOutput.log
		"Checking whether OpenSSL has AES GCM via EVP failed with the following output:\n"
		"${OUTPUT}\n"
		"Source file was:\n${SOURCE}\n"
		)
	set(OPENSSL_HAVE_EVPGCM FALSE)
endif ()

set(SOURCE "
#include <openssl/ec.h>
#include <openssl/ecdh.h>
#include <openssl/ecdsa.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/opensslv.h>
#if OPENSSL_VERSION_NUMBER < 0x0090807f /* 0.9.8g */
#    error \"OpenSSL < 0.9.8g has unreliable ECC code\"
#endif


int main()
{
	EC_KEY *e=EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
	const EVP_MD *m=EVP_sha256(); // We need this too
	return 0;
}
")
file(WRITE "${CMAKE_BINARY_DIR}/CMakeTmp/CheckOpenSSL.c" "${SOURCE}")
message(STATUS "Checking whether OpenSSL has NID_X9_62_prime256v1") #2751
try_compile(OPENSSL_ENABLE_NISTP256
	${CMAKE_BINARY_DIR}
	${CMAKE_BINARY_DIR}/CMakeTmp/CheckOpenSSL.c
	LINK_LIBRARIES ${OPENSSL_LIBRARIES}
	OUTPUT_VARIABLE OUTPUT
	)
if (OPENSSL_ENABLE_NISTP256)
	message(STATUS "Checking whether OpenSSL has NID_X9_62_prime256v1 -- yes")
	set(ENABLE_NISTP256 TRUE)
else ()
	message(STATUS "Checking whether OpenSSL has NID_X9_62_prime256v1 -- no")
	file(APPEND ${CMAKE_BINARY_DIR}/CMakeOutput.log
		"Checking whether OpenSSL has NID_X9_62_prime256v1 failed with the following output:\n"
		"${OUTPUT}\n"
		"Source file was:\n${SOURCE}\n"
		)
	set(ENABLE_NISTP256 FALSE)
endif ()

set(SOURCE "
#include <openssl/ec.h>
#include <openssl/ecdh.h>
#include <openssl/ecdsa.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/opensslv.h>
#if OPENSSL_VERSION_NUMBER < 0x0090807f /* 0.9.8g */
#    error \"OpenSSL < 0.9.8g has unreliable ECC code\"
#endif


int main()
{
	EC_KEY *e=EC_KEY_new_by_curve_name(NID_secp384r1);
	const EVP_MD *m=EVP_sha384(); // We need this too
	return 0;
}
")
file(WRITE "${CMAKE_BINARY_DIR}/CMakeTmp/CheckOpenSSL.c" "${SOURCE}")
message(STATUS "Checking whether OpenSSL has NID_secp384r1") # 2772
try_compile(OPENSSL_ENABLE_NISTP384
	${CMAKE_BINARY_DIR}
	${CMAKE_BINARY_DIR}/CMakeTmp/CheckOpenSSL.c
	LINK_LIBRARIES ${OPENSSL_LIBRARIES}
	OUTPUT_VARIABLE OUTPUT
	)
if (OPENSSL_ENABLE_NISTP384)
	message(STATUS "Checking whether OpenSSL has NID_secp384r1 -- yes")
	set(ENABLE_NISTP384 TRUE)
else ()
	message(STATUS "Checking whether OpenSSL has NID_secp384r1 -- no")
	file(APPEND ${CMAKE_BINARY_DIR}/CMakeOutput.log
		"Checking whether OpenSSL has NID_secp384r1 failed with the following output:\n"
		"${OUTPUT}\n"
		"Source file was:\n${SOURCE}\n"
		)
	set(ENABLE_NISTP384 FALSE)
endif ()

set(SOURCE "
#include <openssl/ec.h>
#include <openssl/ecdh.h>
#include <openssl/ecdsa.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/opensslv.h>
#if OPENSSL_VERSION_NUMBER < 0x0090807f /* 0.9.8g */
#    error \"OpenSSL < 0.9.8g has unreliable ECC code\"
#endif


int main()
{
	EC_KEY *e=EC_KEY_new_by_curve_name(NID_secp521r1);
	const EVP_MD *m=EVP_sha512(); // We need this too
	return 0;
}
")
file(WRITE "${CMAKE_BINARY_DIR}/CMakeTmp/CheckOpenSSL.c" "${SOURCE}")
message(STATUS "Checking whether OpenSSL has NID_secp521r1") # 2793
try_compile(OPENSSL_ENABLE_NISTP521
	${CMAKE_BINARY_DIR}
	${CMAKE_BINARY_DIR}/CMakeTmp/CheckOpenSSL.c
	LINK_LIBRARIES ${OPENSSL_LIBRARIES}
	OUTPUT_VARIABLE OUTPUT
	)
if (OPENSSL_ENABLE_NISTP521)
	message(STATUS "Checking whether OpenSSL has NID_secp521r1 -- yes")
	set(SOURCE "
#include <openssl/ec.h>
#include <openssl/ecdh.h>
#include <openssl/ecdsa.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/opensslv.h>


int main()
{
	EC_KEY *e=EC_KEY_new_by_curve_name(NID_secp521r1);
	const EVP_MD *m=EVP_sha512(); // We need this too
	exit(e==NULL || m==NULL);
}
")
	file(WRITE "${CMAKE_BINARY_DIR}/CMakeTmp/CheckOpenSSL.c" "${SOURCE}")
	message(STATUS "Checking if OpenSSL's NID_secp521r1 is functional") # 2810
	try_run(OPENSSL_ENABLE_NISTP521_FR OPENSSL_ENABLE_NISTP521_FC
		${CMAKE_BINARY_DIR}
		${CMAKE_BINARY_DIR}/CMakeTmp/CheckOpenSSL.c
		LINK_LIBRARIES ${OPENSSL_LIBRARIES}
		COMPILE_OUTPUT_VARIABLE COMPILE_OUTPUT
		RUN_OUTPUT_VARIABLE RUN_OUTPUT
		)
	if (NOT OPENSSL_ENABLE_NISTP521_FC OR OPENSSL_ENABLE_NISTP521_FR)
		message(STATUS "Checking if OpenSSL's NID_secp521r1 is functional -- no")
		file(APPEND ${CMAKE_BINARY_DIR}/CMakeOutput.log
			"Checking if OpenSSL's NID_secp521r1 is functional failed with the following output:\n"
			"${COMPILE_OUTPUT}\n"
			"${RUN_OUTPUT}\n"
			"Source file was:\n${SOURCE}\n"
			)
		set(ENABLE_NISTP521 FALSE)
	else ()
		message(STATUS "Checking if OpenSSL's NID_secp521r1 is functional -- yes")
		set(ENABLE_NISTP521 TRUE)
	endif ()
else ()
	message(STATUS "Checking whether OpenSSL has NID_secp521r1 -- no")
	file(APPEND ${CMAKE_BINARY_DIR}/CMakeOutput.log
		"Checking whether OpenSSL has NID_secp521r1 failed with the following output:\n"
		"${OUTPUT}\n"
		"Source file was:\n${SOURCE}\n"
		)
	set(ENABLE_NISTP521 FALSE)
endif ()

if (ENABLE_NISTP256 OR ENABLE_NISTP384 OR ENABLE_NISTP521) # 2836
	set(OPENSSL_HAS_ECC TRUE)
endif ()
if (ENABLE_NISTP256)
	set(OPENSSL_HAS_NISTP256 TRUE)
endif ()
if (ENABLE_NISTP384)
	set(OPENSSL_HAS_NISTP384 TRUE)
endif ()
if (ENABLE_NISTP521)
	set(OPENSSL_HAS_NISTP521 TRUE)
endif ()

set(SOURCE "
#include <string.h>
#include <openssl/rand.h>


int main()
{
	exit(RAND_status()==1? 0 : 1);
}
")
file(WRITE "${CMAKE_BINARY_DIR}/CMakeTmp/CheckOpenSSL.c" "${SOURCE}")
message(STATUS "Checking whether OpenSSL's PRNG is internally seeded") # 2900
try_run(OPENSSL_SEEDS_ITSELF_FR OPENSSL_SEEDS_ITSELF_FC
	${CMAKE_BINARY_DIR}
	${CMAKE_BINARY_DIR}/CMakeTmp/CheckOpenSSL.c
	LINK_LIBRARIES ${OPENSSL_LIBRARIES}
	COMPILE_OUTPUT_VARIABLE COMPILE_OUTPUT
	RUN_OUTPUT_VARIABLE RUN_OUTPUT
	)
if (NOT OPENSSL_SEEDS_ITSELF_FR OR OPENSSL_SEEDS_ITSELF_FC)
	message(STATUS "Checking whether OpenSSL's PRNG is internally seeded -- yes")
	set(OPENSSL_PRNG_ONLY TRUE)
else ()
	message(STATUS "Checking whether OpenSSL's PRNG is internally seeded -- no")
	file(APPEND ${CMAKE_BINARY_DIR}/CMakeOutput.log
		"Checking whether OpenSSL's PRNG is internally seeded failed with the following output:\n"
		"${COMPILE_OUTPUT}\n"
		"${RUN_OUTPUT}\n"
		"Source file was:\n${SOURCE}\n"
		)
	set(OPENSSL_PRNG_ONLY FALSE)
endif ()

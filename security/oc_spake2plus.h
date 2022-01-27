#ifndef OC_SPAKE2PLUS_H
#define OC_SPAKE2PLUS_H

#include "mbedtls/bignum.h"

int oc_spake_test_vector();
int oc_spake_gen_rnd(oc_string_t *rnd, oc_string_t *salt, int *it);

int oc_spake_init(void);
int oc_spake_free(void);

/**
 * @brief Calculate the w0 & w1 constants
 *
 * Uses PBKDF2 with SHA256 & HMAC to calculate a 40-byte output which is
 * converted into w0 and w1.
 *
 * @param len_pw length of the password
 * @param pw the password
 * @param salt 32-byte array containing the salt
 * @param it the number of iterations to perform within PBKDF2
 * @param w0 the w0 constant as defined by SPAKE2+. Must be initialized by the
 * caller.
 * @param w1 the w1 constant as defined by SPAKE2+. Must be initialized by the
 * caller.
 * @return int 0 on success, mbedtls error code on failure
 */
int oc_spake_calc_w0_w1(size_t len_pw, const uint8_t pw[],
                        const uint8_t salt[32], int it, mbedtls_mpi *w0,
                        mbedtls_mpi *w1);

#endif // OC_SPAKE2PLUS_H
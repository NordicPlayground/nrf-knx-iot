#include "gtest/gtest.h"
#include "mbedtls/md.h"
#include "mbedtls/ecp.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/hkdf.h"
#include "mbedtls/pkcs5.h"

#include "port/oc_random.h"

extern "C" {
#include "security/oc_spake2plus.h"

// Use implementation of Spake2+ with testing context
int calc_transcript_initiator(mbedtls_mpi *w0, mbedtls_mpi *w1, mbedtls_mpi *x,
                              mbedtls_ecp_point *X,
                              const uint8_t Y_enc[kPubKeySize],
                              uint8_t K_main[32], char *idP, char *idV,
                              char *ctx);

int calc_transcript_responder(spake_data_t *spake_data,
                              const uint8_t X_enc[kPubKeySize],
                              mbedtls_ecp_point *Y, char *idP, char *idV,
                              char *ctx);
}

int oc_spake_calc_K_shared_256(uint8_t *K_main, uint8_t K_shared[32]);

static mbedtls_entropy_context entropy_ctx;
static mbedtls_ctr_drbg_context ctr_drbg_ctx;
static mbedtls_ecp_group grp;

class Spake2Plus : public ::testing::Test {
protected:
  virtual void SetUp()
  {
    oc_random_init();
    oc_spake_init();
    int ret = 0;
    // initialize entropy and drbg contexts
    mbedtls_entropy_init(&entropy_ctx);
    mbedtls_ctr_drbg_init(&ctr_drbg_ctx);
    mbedtls_ecp_group_init(&grp);

    MBEDTLS_MPI_CHK(mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1));
    MBEDTLS_MPI_CHK(mbedtls_ctr_drbg_seed(&ctr_drbg_ctx, mbedtls_entropy_func,
                                          &entropy_ctx, NULL, 0));
  cleanup:
    return;
  };

  virtual void TearDown()
  {
    oc_spake_free();
    mbedtls_ctr_drbg_free(&ctr_drbg_ctx);
    mbedtls_entropy_free(&entropy_ctx);
    mbedtls_ecp_group_free(&grp);
    return;
  };
};

// Test Vector values from Spake2+ draft 04 - this is the only test vector that
// uses null identities for A and B
char Context[] = "SPAKE2+-P256-SHA256-HKDF-SHA256-HMAC-SHA256 Test Vectors";
char idProver[] = "client";
char idVerifier[] = "server";

uint8_t bytes_w0[] = { 0xbb, 0x8e, 0x1b, 0xbc, 0xf3, 0xc4, 0x8f, 0x62,
                       0xc0, 0x8d, 0xb2, 0x43, 0x65, 0x2a, 0xe5, 0x5d,
                       0x3e, 0x55, 0x86, 0x05, 0x3f, 0xca, 0x77, 0x10,
                       0x29, 0x94, 0xf2, 0x3a, 0xd9, 0x54, 0x91, 0xb3 };
uint8_t bytes_w1[] = { 0x7e, 0x94, 0x5f, 0x34, 0xd7, 0x87, 0x85, 0xb8,
                       0xa3, 0xef, 0x44, 0xd0, 0xdf, 0x5a, 0x1a, 0x97,
                       0xd6, 0xb3, 0xb4, 0x60, 0x40, 0x9a, 0x34, 0x5c,
                       0xa7, 0x83, 0x03, 0x87, 0xa7, 0x4b, 0x1d, 0xba };
uint8_t bytes_L[] = {
  0x04, 0xeb, 0x7c, 0x9d, 0xb3, 0xd9, 0xa9, 0xeb, 0x1f, 0x8a, 0xda, 0xb8, 0x1b,
  0x57, 0x94, 0xc1, 0xf1, 0x3a, 0xe3, 0xe2, 0x25, 0xef, 0xbe, 0x91, 0xea, 0x48,
  0x74, 0x25, 0x85, 0x4c, 0x7f, 0xc0, 0x0f, 0x00, 0xbf, 0xed, 0xcb, 0xd0, 0x9b,
  0x24, 0x00, 0x14, 0x2d, 0x40, 0xa1, 0x4f, 0x20, 0x64, 0xef, 0x31, 0xdf, 0xaa,
  0x90, 0x3b, 0x91, 0xd1, 0xfa, 0xea, 0x70, 0x93, 0xd8, 0x35, 0x96, 0x6e, 0xfd
};
uint8_t bytes_x[] = { 0xd1, 0x23, 0x2c, 0x8e, 0x86, 0x93, 0xd0, 0x23,
                      0x68, 0x97, 0x6c, 0x17, 0x4e, 0x20, 0x88, 0x85,
                      0x1b, 0x83, 0x65, 0xd0, 0xd7, 0x9a, 0x9e, 0xee,
                      0x70, 0x9c, 0x6a, 0x05, 0xa2, 0xfa, 0xd5, 0x39 };
uint8_t bytes_shareP[] = {
  0x04, 0xef, 0x3b, 0xd0, 0x51, 0xbf, 0x78, 0xa2, 0x23, 0x4e, 0xc0, 0xdf, 0x19,
  0x7f, 0x78, 0x28, 0x06, 0x0f, 0xe9, 0x85, 0x65, 0x03, 0x57, 0x9b, 0xb1, 0x73,
  0x30, 0x09, 0x04, 0x2c, 0x15, 0xc0, 0xc1, 0xde, 0x12, 0x77, 0x27, 0xf4, 0x18,
  0xb5, 0x96, 0x6a, 0xfa, 0xdf, 0xdd, 0x95, 0xa6, 0xe4, 0x59, 0x1d, 0x17, 0x10,
  0x56, 0xb3, 0x33, 0xda, 0xb9, 0x7a, 0x79, 0xc7, 0x19, 0x3e, 0x34, 0x17, 0x27
};
uint8_t bytes_y[] = { 0x71, 0x7a, 0x72, 0x34, 0x8a, 0x18, 0x20, 0x85,
                      0x10, 0x9c, 0x8d, 0x39, 0x17, 0xd6, 0xc4, 0x3d,
                      0x59, 0xb2, 0x24, 0xdc, 0x6a, 0x7f, 0xc4, 0xf0,
                      0x48, 0x32, 0x32, 0xfa, 0x65, 0x16, 0xd8, 0xb3 };
uint8_t bytes_shareV[] = {
  0x04, 0xc0, 0xf6, 0x5d, 0xa0, 0xd1, 0x19, 0x27, 0xbd, 0xf5, 0xd5, 0x60, 0xc6,
  0x9e, 0x1d, 0x7d, 0x93, 0x9a, 0x05, 0xb0, 0xe8, 0x82, 0x91, 0x88, 0x7d, 0x67,
  0x9f, 0xca, 0xde, 0xa7, 0x58, 0x10, 0xfb, 0x5c, 0xc1, 0xca, 0x74, 0x94, 0xdb,
  0x39, 0xe8, 0x2f, 0xf2, 0xf5, 0x06, 0x65, 0x25, 0x5d, 0x76, 0x17, 0x3e, 0x09,
  0x98, 0x6a, 0xb4, 0x67, 0x42, 0xc7, 0x98, 0xa9, 0xa6, 0x84, 0x37, 0xb0, 0x48
};
uint8_t bytes_Z[] = {
  0x04, 0xbb, 0xfc, 0xe7, 0xdd, 0x7f, 0x27, 0x78, 0x19, 0xc8, 0xda, 0x21, 0x54,
  0x4a, 0xfb, 0x79, 0x64, 0x70, 0x55, 0x69, 0xbd, 0xf1, 0x2f, 0xb9, 0x2a, 0xa3,
  0x88, 0x05, 0x94, 0x08, 0xd5, 0x00, 0x91, 0xa0, 0xc5, 0xf1, 0xd3, 0x12, 0x7f,
  0x56, 0x81, 0x3b, 0x53, 0x37, 0xf9, 0xe4, 0xe6, 0x7e, 0x2c, 0xa6, 0x33, 0x11,
  0x7a, 0x4f, 0xbd, 0x55, 0x99, 0x46, 0xab, 0x47, 0x43, 0x56, 0xc4, 0x18, 0x39
};
uint8_t bytes_V[] = {
  0x04, 0x58, 0xbf, 0x27, 0xc6, 0xbc, 0xa0, 0x11, 0xc9, 0xce, 0x19, 0x30, 0xe8,
  0x98, 0x4a, 0x79, 0x7a, 0x34, 0x19, 0x79, 0x7b, 0x93, 0x66, 0x29, 0xa5, 0xa9,
  0x37, 0xcf, 0x2f, 0x11, 0xc8, 0xb9, 0x51, 0x4b, 0x82, 0xb9, 0x93, 0xda, 0x8a,
  0x46, 0xe6, 0x64, 0xf2, 0x3d, 0xb7, 0xc0, 0x1e, 0xdc, 0x87, 0xfa, 0xa5, 0x30,
  0xdb, 0x01, 0xc2, 0xee, 0x40, 0x52, 0x30, 0xb1, 0x89, 0x97, 0xf1, 0x6b, 0x68,
};
uint8_t bytes_K_main[] = { 0x4c, 0x59, 0xe1, 0xcc, 0xf2, 0xcf, 0xb9, 0x61,
                           0xaa, 0x31, 0xbd, 0x94, 0x34, 0x47, 0x8a, 0x10,
                           0x89, 0xb5, 0x6c, 0xd1, 0x15, 0x42, 0xf5, 0x3d,
                           0x35, 0x76, 0xfb, 0x6c, 0x2a, 0x43, 0x8a, 0x29 };
uint8_t K_confirmP[] = { 0x87, 0x1a, 0xe3, 0xf7, 0xb7, 0x84, 0x45, 0xe3,
                         0x44, 0x38, 0xfb, 0x28, 0x45, 0x04, 0x24, 0x02,
                         0x39, 0x03, 0x1c, 0x39, 0xd8, 0x0a, 0xc2, 0x3e,
                         0xb5, 0xab, 0x9b, 0xe5, 0xad, 0x6d, 0xb5, 0x8a };
uint8_t K_confirmV[] = { 0xcc, 0xd5, 0x3c, 0x7c, 0x1f, 0xa3, 0x7b, 0x64,
                         0xa4, 0x62, 0xb4, 0x0d, 0xb8, 0xbe, 0x10, 0x1c,
                         0xed, 0xcf, 0x83, 0x89, 0x50, 0x16, 0x29, 0x02,
                         0x05, 0x4e, 0x64, 0x4b, 0x40, 0x0f, 0x16, 0x80 };
uint8_t HMAC_K_confirmP_shareV[] = { 0x92, 0x6c, 0xc7, 0x13, 0x50, 0x4b, 0x9b,
                                     0x4d, 0x76, 0xc9, 0x16, 0x2d, 0xed, 0x04,
                                     0xb5, 0x49, 0x3e, 0x89, 0x10, 0x9f, 0x6d,
                                     0x89, 0x46, 0x2c, 0xd3, 0x3a, 0xdc, 0x46,
                                     0xfd, 0xa2, 0x75, 0x27 };
uint8_t HMAC_K_confirmV_shareP[] = { 0x97, 0x47, 0xbc, 0xc4, 0xf8, 0xfe, 0x9f,
                                     0x63, 0xde, 0xfe, 0xe5, 0x3a, 0xc9, 0xb0,
                                     0x78, 0x76, 0xd9, 0x07, 0xd5, 0x50, 0x47,
                                     0xe6, 0xff, 0x2d, 0xef, 0x2e, 0x75, 0x29,
                                     0x08, 0x9d, 0x3e, 0x68 };
uint8_t K_shared[] = { 0x0c, 0x5f, 0x8c, 0xcd, 0x14, 0x13, 0x42, 0x3a,
                       0x54, 0xf6, 0xc1, 0xfb, 0x26, 0xff, 0x01, 0x53,
                       0x4a, 0x87, 0xf8, 0x93, 0x77, 0x9c, 0x6e, 0x68,
                       0x66, 0x6d, 0x77, 0x2b, 0xfd, 0x91, 0xf3, 0xe7 };

#define ASSERT_RET(x)                                                          \
  do {                                                                         \
    ASSERT_EQ(x, 0);                                                           \
  } while (0)
#define EXPECT_RET(x)                                                          \
  do {                                                                         \
    EXPECT_EQ(x, 0);                                                           \
  } while (0)

TEST_F(Spake2Plus, CalculatePublicA)
{
  // =========================
  // Check that X = x*P + w0*M
  // =========================
  mbedtls_mpi x, w0;
  mbedtls_mpi_init(&x);
  mbedtls_mpi_init(&w0);
  uint8_t cmpbuf[128];
  size_t cmplen;

  ASSERT_RET(mbedtls_mpi_read_binary(&x, bytes_x, sizeof(bytes_x)));
  ASSERT_RET(mbedtls_mpi_read_binary(&w0, bytes_w0, sizeof(bytes_w0)));

  mbedtls_ecp_point X, pubA;
  mbedtls_ecp_point_init(&X);
  mbedtls_ecp_point_init(&pubA);
  // pubA = x*P (P is the generator group element, mbedtls uses G)
  ASSERT_RET(mbedtls_ecp_mul(&grp, &pubA, &x, &grp.G, mbedtls_ctr_drbg_random,
                             &ctr_drbg_ctx));

  // X = pubA + w0*M
  ASSERT_RET(oc_spake_calc_shareP(&X, &pubA, &w0));
  ASSERT_RET(mbedtls_ecp_point_write_binary(
    &grp, &X, MBEDTLS_ECP_PF_UNCOMPRESSED, &cmplen, cmpbuf, sizeof(cmpbuf)));

  // check the value of X is correct
  ASSERT_TRUE(0 == memcmp(bytes_shareP, cmpbuf, cmplen));
  mbedtls_mpi_free(&x);
  mbedtls_mpi_free(&w0);
}

TEST_F(Spake2Plus, CalculatePublicB)
{
  // =========================
  // Check that Y = y*P + w0*N
  // =========================
  mbedtls_mpi y, w0;
  mbedtls_mpi_init(&y);
  mbedtls_mpi_init(&w0);
  uint8_t cmpbuf[128];
  size_t cmplen;

  ASSERT_RET(mbedtls_mpi_read_binary(&w0, bytes_w0, sizeof(bytes_w0)));

  ASSERT_RET(mbedtls_mpi_read_binary(&y, bytes_y, sizeof(bytes_y)));

  mbedtls_ecp_point Y, pubB;
  mbedtls_ecp_point_init(&Y);
  mbedtls_ecp_point_init(&pubB);
  // pubB = y*P
  ASSERT_RET(mbedtls_ecp_mul(&grp, &pubB, &y, &grp.G, mbedtls_ctr_drbg_random,
                             &ctr_drbg_ctx));

  // Y = pubB + w0*N
  ASSERT_RET(oc_spake_calc_shareV(&Y, &pubB, &w0));
  ASSERT_RET(mbedtls_ecp_point_write_binary(
    &grp, &Y, MBEDTLS_ECP_PF_UNCOMPRESSED, &cmplen, cmpbuf, sizeof(cmpbuf)));
  // check the value of Y is correct
  ASSERT_TRUE(memcmp(bytes_shareV, cmpbuf, cmplen) == 0);

  mbedtls_mpi_free(&y);
  mbedtls_mpi_free(&w0);
}

TEST_F(Spake2Plus, CalculateSecretA)
{
  // =================================
  // Checks values of Z, V, TT & K_main
  // for Party A (the initiator)
  // =================================
  mbedtls_mpi w0, w1, x;
  mbedtls_ecp_point X;
  uint8_t K_main[32];
  uint8_t shared_key[32];

  mbedtls_mpi_init(&w0);
  mbedtls_mpi_init(&w1);
  mbedtls_mpi_init(&x);
  mbedtls_ecp_point_init(&X);

  ASSERT_RET(mbedtls_mpi_read_binary(&w0, bytes_w0, sizeof(bytes_w0)));
  ASSERT_RET(mbedtls_mpi_read_binary(&w1, bytes_w1, sizeof(bytes_w1)));
  ASSERT_RET(mbedtls_mpi_read_binary(&x, bytes_x, sizeof(bytes_x)));
  ASSERT_RET(mbedtls_ecp_point_read_binary(&grp, &X, bytes_shareP,
                                           sizeof(bytes_shareP)));

  ASSERT_RET(calc_transcript_initiator(&w0, &w1, &x, &X, bytes_shareV, K_main,
                                       idProver, idVerifier, Context));
  ASSERT_RET(memcmp(bytes_K_main, K_main, sizeof(bytes_K_main)));

  ASSERT_RET(oc_spake_calc_K_shared_256(K_main, shared_key));
  ASSERT_RET(memcmp(shared_key, K_shared, sizeof(K_shared)));

  mbedtls_mpi_free(&w0);
  mbedtls_mpi_free(&w1);
  mbedtls_mpi_free(&x);
  mbedtls_ecp_point_free(&X);
}

TEST_F(Spake2Plus, CalculateSecretB)
{
  // =================================
  // Checks values of Z, V, TT & K_main
  // for Party B (the responder)
  // =================================

  // input: y, Y, w0, L
  spake_data_t spake_data;
  uint8_t shared_key[32];
  mbedtls_ecp_point Y;
  mbedtls_mpi_init(&spake_data.y);
  mbedtls_mpi_init(&spake_data.w0);
  mbedtls_ecp_point_init(&spake_data.L);
  mbedtls_ecp_point_init(&Y);

  ASSERT_RET(mbedtls_mpi_read_binary(&spake_data.y, bytes_y, sizeof(bytes_y)));
  ASSERT_RET(
    mbedtls_mpi_read_binary(&spake_data.w0, bytes_w0, sizeof(bytes_w0)));
  ASSERT_RET(mbedtls_ecp_point_read_binary(&grp, &spake_data.L, bytes_L,
                                           sizeof(bytes_L)));
  ASSERT_RET(mbedtls_ecp_point_read_binary(&grp, &Y, bytes_shareV,
                                           sizeof(bytes_shareV)));

  ASSERT_RET(calc_transcript_responder(&spake_data, bytes_shareP, &Y, idProver,
                                       idVerifier, Context));
  ASSERT_RET(memcmp(bytes_K_main, spake_data.K_main, sizeof(bytes_K_main)));

  ASSERT_RET(oc_spake_calc_K_shared_256(spake_data.K_main, shared_key));
  ASSERT_RET(memcmp(shared_key, K_shared, sizeof(K_shared)));

  mbedtls_mpi_free(&spake_data.y);
  mbedtls_mpi_free(&spake_data.w0);
  mbedtls_ecp_point_free(&spake_data.L);
  mbedtls_ecp_point_free(&Y);
}

TEST_F(Spake2Plus, CalculateConfirmationA)
{
  uint8_t calculated_confirmP[32];

  ASSERT_RET(
    oc_spake_calc_confirmP(bytes_K_main, calculated_confirmP, bytes_shareV));
  EXPECT_TRUE(memcmp(HMAC_K_confirmP_shareV, calculated_confirmP,
                     sizeof(HMAC_K_confirmP_shareV)) == 0);
}

TEST_F(Spake2Plus, CalculateConfirmationB)
{
  uint8_t calculated_confirmV[32];

  ASSERT_RET(
    oc_spake_calc_confirmV(bytes_K_main, calculated_confirmV, bytes_shareP));
  EXPECT_TRUE(memcmp(HMAC_K_confirmV_shareP, calculated_confirmV,
                     sizeof(HMAC_K_confirmV_shareP)) == 0);
}

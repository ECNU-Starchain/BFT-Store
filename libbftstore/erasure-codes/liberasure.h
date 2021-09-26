#pragma once

#include <stdint.h>
#include <stddef.h>

#if defined(__arm__) || defined(__TARGET_ARCH_ARM)
#if defined(__ARM_ARCH_7__) \\
        || defined(__ARM_ARCH_7A__) \\
        || defined(__ARM_ARCH_7R__) \\
        || defined(__ARM_ARCH_7M__) \\
        || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 7)
#define ERASURE_ARCH_ARMV7
#elif defined(__ARM_ARCH_6__) \\
        || defined(__ARM_ARCH_6J__) \\
        || defined(__ARM_ARCH_6T2__) \\
        || defined(__ARM_ARCH_6Z__) \\
        || defined(__ARM_ARCH_6K__) \\
        || defined(__ARM_ARCH_6ZK__) \\
        || defined(__ARM_ARCH_6M__) \\
        || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 6)
#define ERASURE_ARCH_ARMV6
#elif defined(__ARM_ARCH_5TEJ__) \\
        || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 5)
#define ERASURE_ARCH_ARMV5
#else
#define ERASURE_ARCH_ARM
#endif
#elif defined(__i386) || defined(__i386__) || defined(_M_IX86)
#define ERASURE_ARCH_I386
#elif defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64)
#define ERASURE_ARCH_X86_64
#elif defined(__ia64) || defined(__ia64__) || defined(_M_IA64)
#define ERASURE_ARCH_IA64

#else
#define ERASURE_ARCH_UNKNOWN
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct erasure_encoder_ erasure_encoder;
typedef struct erasure_encode_stream_ erasure_encode_stream;
typedef struct erasure_recover_stream_ erasure_recover_stream;

typedef char erasure_bool;

typedef struct {
	uint8_t n; // Total number of shards
	uint8_t k; // Number of data shards
	size_t data_size;
} erasure_encoder_parameters;

enum erasure_encoder_flags
{
	ERASURE_DEFAULT = 0,
	ERASURE_FORCE_REF_IMPL = 1,
	ERASURE_FORCE_ADV_IMPL = 2,
#if defined ERASURE_ARCH_I386 || defined ERASURE_ARCH_X86_64
	ERASURE_FORCE_SSSE3_IMPL = 3,
	ERASURE_FORCE_AVX2_IMPL = 4,
#endif
};

enum erasure_error_code
{
	ERASURE_SUCCESS = 0,
	ERASURE_RECOVER_FAILED,
	ERASURE_INVALID_ARGUMENTS,
	ERASURE_INTERNAL_ERROR,
};
	
/* Block API */
erasure_encoder* erasure_create_encoder(
	const erasure_encoder_parameters* params,
	enum erasure_encoder_flags flags);
void erasure_destroy_encoder(erasure_encoder* encoder);

enum erasure_error_code erasure_encode(
	erasure_encoder* encoder,
	const uint8_t* const* shards,
	uint8_t* const* parity);

enum erasure_error_code erasure_encode_partial(
	erasure_encoder* encoder,
	const uint8_t* const* shards,
	uint8_t* const* parity,
	const erasure_bool* present);

enum erasure_error_code erasure_recover_data(
	erasure_encoder* encoder,
	uint8_t* const* shards,
	const erasure_bool* present);

enum erasure_error_code erasure_recover(
	erasure_encoder* encoder,
	uint8_t* const* shards,
	const erasure_bool* present);

/* Stream API */

erasure_encode_stream* erasure_create_encode_stream(
	erasure_encoder* encoder,
	const erasure_bool* should_encode);
erasure_recover_stream* erasure_create_recover_stream(
	erasure_encoder* encoder,
	const erasure_bool* present);

void erasure_destroy_encode_stream(erasure_encode_stream* stream);
void erasure_destroy_recover_stream(erasure_recover_stream* stream);

enum erasure_error_code erasure_stream_encode(
	erasure_encode_stream* stream,
	const uint8_t* const* shards,
	uint8_t* const* parity);
enum erasure_error_code erasure_stream_recover_data(
	erasure_recover_stream* stream,
	uint8_t* const* shards);

#ifdef __cplusplus
}
#endif
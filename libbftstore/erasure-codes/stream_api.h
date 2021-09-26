#include "liberasure.h"

namespace erasure
{
	typedef erasure_encoder_ rs_encoder;
	typedef erasure_encode_stream_ encode_stream;
	typedef erasure_recover_stream_ recover_stream;
	typedef erasure_error_code error_code;

	static constexpr auto SUCCESS = ERASURE_SUCCESS;
	static constexpr auto RECOVER_FAILED = ERASURE_RECOVER_FAILED;
	static constexpr auto INVALID_ARGUMENTS = ERASURE_INVALID_ARGUMENTS;
	static constexpr auto INTERNAL_ERROR = ERASURE_INTERNAL_ERROR;
	
	recover_stream* create_recover_stream(
		rs_encoder* encoder,
		const bool* present);
	encode_stream* create_encode_stream(
		rs_encoder* encoder,
		const bool* should_encode);
	void destroy_stream(recover_stream* stream);
	void destroy_stream(encode_stream* stream);

	error_code stream_encode(
		encode_stream* stream,
		const uint8_t* const* shards,
		uint8_t* const* parity);
	error_code stream_recover_data(
		recover_stream* stream,
		uint8_t* const* shards);
}

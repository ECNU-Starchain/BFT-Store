
#include "stream_api.h"

namespace erasure
{

	typedef erasure_encoder_parameters encode_parameters;
	typedef erasure_encoder_flags encoder_flags;
	
	static constexpr auto DEFAULT = ERASURE_DEFAULT;
	static constexpr auto USE_REF_IMPL = ERASURE_FORCE_REF_IMPL;
	static constexpr auto USE_ADV_IMPL = ERASURE_FORCE_ADV_IMPL;
#if defined ERASURE_ARCH_I386 || defined ERASURE_ARCH_X86_64
	static constexpr auto USE_SSSE3_IMPL = ERASURE_FORCE_SSSE3_IMPL;
	static constexpr auto USE_AVX2_IMPL = ERASURE_FORCE_AVX2_IMPL;
#endif

	rs_encoder* create_encoder(
		const encode_parameters& params,
		encoder_flags flags = ERASURE_DEFAULT);
	void destroy_encoder(rs_encoder* encoder);

	error_code encode(
		rs_encoder* encoder,
		const uint8_t* const* shards,
		uint8_t* const* parity);

	/* Encodes a subset of the parity shards given
	   the data shards. Unused parity shard pointers
	   are not accessed and may be null.
	   
	   PARAMETERS:
	   encoder: 
	      An encoder instance.
	   shards:  
	      The data shards from which the parity
	      shards are calculated.
	   parity:
	      The parity shards.
	   should_encode:
	      A boolean array specifying which parity
	      shards are to be calculated.
	*/
	error_code encode_partial(
		rs_encoder* encoder,
		const uint8_t* const* shards,
		uint8_t* const* parity,
		const bool* should_encode);

	error_code recover_data(
		rs_encoder* encoder,
		uint8_t* const* shards,
		const bool* present);
	error_code recover(
		rs_encoder* encoder,
		uint8_t* const* shards,
		const bool* present);
}

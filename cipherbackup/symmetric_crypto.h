#pragma once

#include "padding/padding.h"
#include "block_cipher.h"
#include "xor.h"
#include "ecb.h"
#include "cbc.h"
#include "ctr.h"
#include "speck.h"
#include "aes.h"
#include "rc6.h"

/*namespace fast_io::crypto
{

template<input_stream in>
using ictr_speck=basic_ictr<in,speck::speck_enc_128_256>;

template<output_stream out>
using octr_speck=basic_octr<out,speck::speck_enc_128_256>;

template<input_stream in>
using icbc_speck=basic_ictr<in,speck::speck_dec_128_256>;

template<output_stream out>
using ocbc_speck=basic_octr<out,speck::speck_enc_128_256>;

}*/
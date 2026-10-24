/* Xbox ADPCM block decoding; see xbox_adpcm.hpp for the layout and sources. */
#include "audio/xbox_adpcm.hpp"

namespace h2::audio {
namespace {

/* IMA ADPCM step sizes and index adjustments (IMA recommended practice 1992) */
constexpr int k_index_table[16] = {-1, -1, -1, -1, 2, 4, 6, 8,
                                   -1, -1, -1, -1, 2, 4, 6, 8};
constexpr int k_step_table[89] = {
    7,     8,     9,     10,    11,    12,    13,    14,    16,    17,    19,    21,    23,
    25,    28,    31,    34,    37,    41,    45,    50,    55,    60,    66,    73,    80,
    88,    97,    107,   118,   130,   143,   157,   173,   190,   209,   230,   253,   279,
    307,   337,   371,   408,   449,   494,   544,   598,   658,   724,   796,   876,   963,
    1060,  1166,  1282,  1411,  1552,  1707,  1878,  2066,  2272,  2499,  2749,  3024,  3327,
    3660,  4026,  4428,  4871,  5358,  5894,  6484,  7132,  7845,  8630,  9493,  10442, 11487,
    12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767};

}  // namespace

std::int16_t ima_adpcm_step(std::uint8_t code, int& predictor, int& index) {
    const int step = k_step_table[index];
    int difference = step >> 3;
    if (code & 1)
        difference += step >> 2;
    if (code & 2)
        difference += step >> 1;
    if (code & 4)
        difference += step;
    if (code & 8)
        difference = -difference;
    predictor += difference;
    if (predictor > 32767)
        predictor = 32767;
    else if (predictor < -32768)
        predictor = -32768;
    index += k_index_table[code & 15];
    if (index < 0)
        index = 0;
    else if (index > 88)
        index = 88;
    return static_cast<std::int16_t>(predictor);
}

void xbox_adpcm_decode_block(const std::uint8_t* block, unsigned channels,
                             std::int16_t* frames) {
    if (!channels || channels > 8)
        return;
    int predictor[8];
    int index[8];
    for (unsigned c = 0; c < channels; ++c) {
        const std::uint8_t* header = block + c * 4;
        predictor[c] = static_cast<std::int16_t>(header[0] | (header[1] << 8));
        index[c] = header[2] > 88 ? 88 : header[2];
    }
    /* the data: 8 groups of 4 bytes per channel, channel-interleaved */
    const std::uint8_t* data = block + channels * 4;
    for (unsigned group = 0; group < 8; ++group) {
        for (unsigned c = 0; c < channels; ++c) {
            const std::uint8_t* bytes = data + (group * channels + c) * 4;
            for (unsigned b = 0; b < 4; ++b) {
                const unsigned sample = group * 8 + b * 2;
                frames[sample * channels + c] =
                    ima_adpcm_step(bytes[b] & 15, predictor[c], index[c]);
                frames[(sample + 1) * channels + c] =
                    ima_adpcm_step(bytes[b] >> 4, predictor[c], index[c]);
            }
        }
    }
}

}  // namespace h2::audio

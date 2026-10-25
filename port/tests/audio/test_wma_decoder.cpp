/* Tests for the host WMA decoder (port/src/audio/wma_decoder.cpp) behind the
 * guest's in-memory WMA decoder object.
 *
 * Usage: test_wma_decoder WORK_DIR
 *
 * No game data: the ffmpeg command line encodes test tones as WMA in ASF,
 * the container the title streams, and the decoder must hand them back as
 * 16-bit 44.1 kHz stereo at the right length and pitch. Without ffmpeg on
 * PATH the test reports itself skipped (exit 77, CTest's skip code). */
#include "h2/host_abi.h"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace {

int failures = 0;
int checks = 0;

void check(bool condition, const std::string& what) {
    ++checks;
    if (!condition) {
        ++failures;
        std::printf("FAIL: %s\n", what.c_str());
    }
}

/* encodes a sine tone with the ffmpeg CLI; false when that is not possible */
bool encode_tone(const std::string& path, int frequency, int rate, int channels, double seconds) {
    const std::string command = "ffmpeg -hide_banner -loglevel error -y -f lavfi -i \"sine=frequency=" +
                                std::to_string(frequency) + ":sample_rate=" + std::to_string(rate) +
                                "\" -t " + std::to_string(seconds) + " -ac " + std::to_string(channels) +
                                " -c:a wmav2 -b:a 128k -f asf \"" + path + "\"";
    return std::system(command.c_str()) == 0;
}

std::vector<std::uint8_t> read_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}

/* pulls everything in the guest's packet size; returns interleaved samples */
std::vector<std::int16_t> decode_all(unsigned handle) {
    std::vector<std::int16_t> samples;
    std::vector<std::int16_t> packet(0x10000 / 2);
    for (;;) {
        const unsigned produced = host_h2_wma_decode(handle, packet.data(), 0x10000);
        samples.insert(samples.end(), packet.begin(), packet.begin() + produced / 2);
        if (produced < 0x10000)
            break;
    }
    return samples;
}

/* the tone's frequency from upward zero crossings of one channel */
double measured_frequency(const std::vector<std::int16_t>& samples, int channel) {
    long crossings = 0;
    const std::size_t frames = samples.size() / 2;
    for (std::size_t i = 1; i < frames; ++i)
        if (samples[(i - 1) * 2 + channel] < 0 && samples[i * 2 + channel] >= 0)
            ++crossings;
    return crossings * 44100.0 / static_cast<double>(frames);
}

void tone_case(const std::string& dir, const char* name, int frequency, int rate, int channels) {
    const std::string path = dir + "/" + name + ".wma";
    if (!encode_tone(path, frequency, rate, channels, 1.0)) {
        check(false, std::string(name) + ": ffmpeg encode");
        return;
    }
    const std::vector<std::uint8_t> data = read_file(path);
    check(!data.empty(), std::string(name) + ": encoded file read");
    const unsigned handle = host_h2_wma_open(data.data(), static_cast<unsigned>(data.size()));
    check(handle != 0, std::string(name) + ": decoder opens the ASF file");
    if (!handle)
        return;
    const std::vector<std::int16_t> samples = decode_all(handle);
    host_h2_wma_close(handle);

    /* one second of 44.1 kHz stereo, give or take the codec's padding */
    const double seconds = samples.size() / 2 / 44100.0;
    check(seconds > 0.9 && seconds < 1.15,
          std::string(name) + ": length " + std::to_string(seconds) + " s at 44.1 kHz");
    int peak = 0;
    for (std::int16_t s : samples)
        peak = std::max(peak, std::abs(static_cast<int>(s)));
    /* lavfi's sine is at 1/8 of full scale, a peak near 4096 */
    check(peak > 2000 && peak < 6000, std::string(name) + ": tone level (peak " + std::to_string(peak) + ")");
    const double left = measured_frequency(samples, 0);
    check(std::fabs(left - frequency) < frequency * 0.03,
          std::string(name) + ": pitch " + std::to_string(left) + " Hz, wanted " + std::to_string(frequency));
    if (channels == 1) {
        /* mono is duplicated into both channels */
        bool same = true;
        for (std::size_t i = 0; i + 1 < samples.size(); i += 2)
            same = same && samples[i] == samples[i + 1];
        check(same, std::string(name) + ": mono duplicated to both channels");
    }
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        std::printf("usage: %s WORK_DIR\n", argv[0]);
        return 2;
    }
    if (std::system("ffmpeg -hide_banner -version > /dev/null 2>&1") != 0) {
        std::printf("SKIP: no ffmpeg on PATH to encode test tones\n");
        return 77;
    }
    const std::string dir = argv[1];
    /* the title's music: 44.1 kHz stereo */
    tone_case(dir, "stereo_44100", 440, 44100, 2);
    /* other rates and mono go through the resampler and channel duplication */
    tone_case(dir, "mono_22050", 1000, 22050, 1);
    tone_case(dir, "stereo_32000", 523, 32000, 2);

    /* bad input fails cleanly: no handle, nothing decoded */
    const std::uint8_t junk[64] = {1, 2, 3};
    check(host_h2_wma_open(junk, sizeof(junk)) == 0, "junk data does not open");
    check(host_h2_wma_open(nullptr, 100) == 0, "null data does not open");
    std::int16_t out[4];
    check(host_h2_wma_decode(12345, out, sizeof(out)) == 0, "unknown handle decodes nothing");
    host_h2_wma_close(12345);

    std::printf("%d checks, %d failures\n", checks, failures);
    return failures ? 1 : 0;
}

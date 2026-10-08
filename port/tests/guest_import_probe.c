#include "h2/host_abi.h"

int h2_guest_import_probe(const char* message) {
    if (host_h2_abi_version() != H2_HOST_ABI_VERSION) {
        return -1;
    }
    host_h2_log(message);
    return 0;
}

#ifndef H2_PORT_XBOX_SYSTEM_HPP
#define H2_PORT_XBOX_SYSTEM_HPP

#include <filesystem>
#include <string>

namespace h2::port {

class GuestArena;
class GuestElfImage;

/* Native services behind the recovered game's Xbox system calls in a whole
   image: virtual and contiguous memory, the title's drives (d: the user's game
   files, read only; t:, u:, z: and the cache drives in a private directory),
   directory search and clocks. */
bool configure_xbox_system(GuestArena& arena, const std::filesystem::path& data_root,
                           const std::filesystem::path& user_root, std::string& error);
bool patch_xbox_system_imports(GuestElfImage& image, GuestArena& arena, std::string& error);

} // namespace h2::port

#endif

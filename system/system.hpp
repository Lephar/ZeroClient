#ifndef ZERO_CLIENT_SYSTEM_HPP
#define ZERO_CLIENT_SYSTEM_HPP

#include "base.hpp"
#include "window.hpp"

namespace zero::system {
    class System {
    private:
        xcb_connection_t *connection;
        xcb_screen_t *screen;

        std::unordered_map<xcb_window_t, Window> windows;

        xcb_atom_t protocol;
        xcb_atom_t destroyEvent;

        xcb_atom_t requestAtom(const std::string &message);

        void processEvents();

    public:
        System();

        xcb_connection_t *getConnection();

        xcb_window_t createWindow(const std::string &title, uint16_t width, uint16_t height);

        void mainLoop();

        void destroyWindow(uint32_t index);

        ~System();
    };
}

#endif //ZERO_CLIENT_SYSTEM_HPP

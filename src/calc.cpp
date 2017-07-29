#include <expression.hpp>
#include <gtkmm.h>
#include <gui.hpp>

int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create(argc, argv);
    GUI  gui;
    return app->run(*gui.windowRoot);
}

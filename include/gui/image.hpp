#pragma once
#include <array>
#include <gtkmm.h>
#include <gui/coord.hpp>

class Image {
  public:
    Glib::RefPtr<Gdk::Pixbuf> pixbuf;
    std::array<int, 3> brush;

    Image();
    Image(const int width, const int height);

    void set_pixel(const int x, const int y, const std::array<int, 3> rgb);
    void set_pixel(const int x, const int y);
    void draw_char(const char c, const int x, const int y);
    void draw_string(const Glib::ustring s, const long double x, const long double y);
    void flip(const bool horizontally);
    void clear();
    Glib::RefPtr<Gdk::Pixbuf> draw_functions(Glib::RefPtr<Gtk::ListStore> function_store, const int width, const int height, const Coord scale, const Coord offset);
};

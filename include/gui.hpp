#pragma once
#include <array>
#include <atomic>
#include <expression.hpp>
#include <gtkmm.h>
#include <queue>

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
    Glib::RefPtr<Gdk::Pixbuf> draw_functions(Glib::RefPtr<Gtk::ListStore> function_store, const int width, const int height, const int scale_x, const int scale_y);
};

class FunctionColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<Glib::ustring> text;
    Gtk::TreeModelColumn<Operand*>      function;
    Gtk::TreeModelColumn<double>        zero;

    FunctionColumns();
};

class GUI {
  public:
    Image                                 image;
    Glib::RefPtr<Gtk::Builder>            builder;
    Gtk::Window*                          window_root;
    Gtk::Grid*                            grid_main;
    Gtk::Entry*                           entry_function;
    Gtk::DrawingArea*                     drawing_area;
    Gtk::Scale*                           scale_x;
    Gtk::Scale*                           scale_y;
    Gtk::TextView*                        text_view_results;
    Gtk::TreeView*                        tree_view_results;
    FunctionColumns                       function_columns;
    Glib::RefPtr<Gtk::ListStore>          function_store;
    Gtk::Button*                          button_derive;
    Gtk::Button*                          button_delete;
    int                                   x_scale_val;
    int                                   y_scale_val;
    int                                   next_name;
    int                                   drag_start;
    bool                                  dragging_x;
    bool                                  dragging_y;
    std::atomic<bool>                     working;
    std::queue<Glib::RefPtr<Gdk::Pixbuf>> render_queue;

    GUI();
    ~GUI();

    /**
     * @brief adapt canvas size to window size and draw graph according to f(x)
     */
    void redraw();
    void drag_update(int x, int y);
};


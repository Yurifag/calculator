#pragma once
#include <array>
#include <atomic>
#include <expression.hpp>
#include <gtkmm.h>

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
    Glib::RefPtr<Gdk::Pixbuf> draw_functions(Glib::RefPtr<Gtk::ListStore> functionStore, const int width, const int height, const int scale_x, const int scale_y);
};

class FunctionColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<Glib::ustring> text;
    Gtk::TreeModelColumn<Operand*>   function;
    Gtk::TreeModelColumn<double>        zero;

    FunctionColumns();
};

class GUI {
  public:
    Image                        image;
    Glib::RefPtr<Gtk::Builder>   builder;
    Gtk::Window                  *windowRoot;
    Gtk::Grid                    *gridMain;
    Gtk::Entry                   *entryFunction;
    Gtk::DrawingArea             *drawingArea;
    Gtk::Scale                   *scaleX;
    Gtk::Scale                   *scaleY;
    Gtk::TextView                *textViewResults;
    Gtk::TreeView                *treeViewResults;
    FunctionColumns              functionColumns;
    Glib::RefPtr<Gtk::ListStore> functionStore;
    Gtk::Button                  *buttonDerive;
    Gtk::Button                  *buttonDelete;
    int                          x_scale;
    int                          y_scale;
    int                          next_name;
    std::atomic<bool>            working;

    GUI();
    ~GUI();

    /**
     * @brief adapt canvas size to window size and draw graph according to f(x)
     */
    void redraw();
};


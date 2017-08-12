#pragma once
#include <atomic>
#include <gtkmm.h>
#include <gui/coord.hpp>
#include <gui/image.hpp>
#include <gui/model/function_model.hpp>
#include <map>

class GUI {
  public:
    enum DRAG_EVENT {
        SCALE_X,
        SCALE_Y,
        MOVE,
        NONE
    };

    Image                                 image;
    Glib::RefPtr<Gtk::Builder>            builder;
    Gtk::Window*                          window_root;
    Gtk::Grid*                            grid_main;
    Gtk::Entry*                           entry_function;
    Gtk::DrawingArea*                     drawing_area;
    Gtk::TextView*                        text_view_results;
    Gtk::TreeView*                        tree_view_results;
    FunctionColumns                       function_columns;
    Glib::RefPtr<Gtk::ListStore>          function_store;
    Gtk::Button*                          button_derive;
    Gtk::Button*                          button_delete;
    int                                   next_name;
    Coord                                 scale;
    Coord                                 drag_start;
    Coord                                 grid_offset;
    long double                           drag_start_scale;
    std::atomic<bool>                     working;
    DRAG_EVENT                            drag_event_id;

    GUI();
    ~GUI();

    /**
     * @brief adapt canvas size to window size and draw graph according to f(x)
     */
    void redraw();
    void drag_update(int x, int y);
};

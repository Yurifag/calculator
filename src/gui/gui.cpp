#include <expression.hpp>
#include <gtkmm.h>
#include <gui/gui.hpp>
#include <gui/model/function_model.hpp>
#include <parser.hpp>
#include <string>
#include <thread>
#include <util.hpp>

using namespace util;

GUI::GUI() : next_name(102), scale(1.0L / 30.0L, 1.0L / 30.0L),  drag_start(0, 0), grid_offset(0, 0), drag_start_scale(1), working(false), drag_event_id(DRAG_EVENT::NONE) {
    // --- setup GUI elements ---
    this->builder = Gtk::Builder::create_from_resource("/org/gtk/calc/calc.glade");
    this->builder->get_widget("window_root",       this->window_root);
    this->builder->get_widget("entry_function",    this->entry_function);
    this->builder->get_widget("drawing_area",      this->drawing_area);
    this->builder->get_widget("text_view_results", this->text_view_results);
    this->builder->get_widget("tree_view_results", this->tree_view_results);
    this->builder->get_widget("button_derive",     this->button_derive);
    this->builder->get_widget("button_delete",     this->button_delete);

    // --- setup view models ---
    this->function_store = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(this->builder->get_object("function_store"));
    this->tree_view_results->set_model(this->function_store);

    // --- setup widget style ---
    Glib::RefPtr<Gtk::CssProvider> css = Gtk::CssProvider::create();
    css->load_from_resource("/org/gtk/calc/calc.css");
    Glib::RefPtr<Gtk::StyleContext> style = Gtk::StyleContext::create();
    style->add_provider_for_screen(this->window_root->get_screen(), css, GTK_STYLE_PROVIDER_PRIORITY_USER);

    // --- setup event handlers ---
    this->entry_function->signal_key_release_event().connect([this] (GdkEventKey *key) {
        this->entry_function->get_style_context()->remove_class("invalid");
        Glib::ustring text = this->entry_function->get_text();
        if(key->keyval == KEYCODE_ENTER) {
            if(!text.empty() && Parser::validate(text)) {
                Gtk::TreeRow row = *this->function_store->append();
                row[this->function_columns.name] = std::string(1, this->next_name);
                this->next_name++;
                row[this->function_columns.function] = Parser::parse(text)->simplify();
                row[this->function_columns.text] = (*row[this->function_columns.function]).to_string();

                this->entry_function->set_text("");
                this->redraw();
            }
            else {
                this->entry_function->get_style_context()->add_class("invalid");
            }
        }
        return false;
    });

    this->button_derive->signal_clicked().connect([this] () {
        Gtk::TreeRow row = *this->tree_view_results->get_selection()->get_selected();
        if(row) {
            this->button_derive->set_state(Gtk::STATE_INSENSITIVE);
            std::thread derive([this, row] () {
                Operand* derivative = ((Operand*)row[this->function_columns.function])->derive();
                Glib::ustring name = row[this->function_columns.name] + "'";
                Gtk::TreeRow append = *this->function_store->append();
                append[this->function_columns.name] = name;
                append[this->function_columns.text] = derivative->to_string();
                append[this->function_columns.function] = derivative;
                this->redraw();
                Glib::signal_idle().connect([this] () {
                    this->button_derive->set_state(Gtk::STATE_NORMAL);
                    return false;
                });
            });
            derive.detach();
        }
    });

    this->button_delete->signal_clicked().connect([this]() {
        Gtk::TreeIter row = this->tree_view_results->get_selection()->get_selected();
        if(row) {
            this->function_store->erase(row);
            this->redraw();
        }
    });

    // --- setup drawing area events ---
    // listen to these additional events
    this->drawing_area->add_events(Gdk::EventMask::POINTER_MOTION_MASK | Gdk::EventMask::BUTTON_PRESS_MASK | Gdk::EventMask::BUTTON_RELEASE_MASK);

    // redraw on (window) resize
    this->drawing_area->signal_size_allocate().connect([this](Gdk::Rectangle allocation) {
        this->image.pixbuf = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, 0, 8, allocation.get_width(), allocation.get_height());
        this->redraw();
    });

    // copy pixel data to drawing area context
    this->drawing_area->signal_draw().connect([this](const Cairo::RefPtr<Cairo::Context> &context) {
        Gdk::Cairo::set_source_pixbuf(context, this->image.pixbuf, 0, 0);
        context->rectangle(0, 0, this->image.pixbuf->get_width(), this->image.pixbuf->get_height());
        context->fill();
        return true;
    });

    this->drawing_area->signal_motion_notify_event().connect([this] (GdkEventMotion* motion) {
        if(this->drag_event_id == DRAG_EVENT::NONE) {
            // change pointer style when hovering over drawing area

            int height = this->image.pixbuf->get_height();
            int width = this->image.pixbuf->get_width();

            Coord rel_pos = Coord(motion->x + this->grid_offset.x, motion->y + this->grid_offset.y);

            if(rel_pos.y > height / 2 - 20 && rel_pos.y < height / 2 + 20 && rel_pos.x > width / 2 - 20 && rel_pos.x < width / 2 + 20) {
                this->drawing_area->get_window()->set_cursor(Gdk::Cursor::create(Gdk::CursorType::FLEUR));
            }
            else if(rel_pos.y > height / 2 - 5 && rel_pos.y < height / 2 + 5) {
                this->drawing_area->get_window()->set_cursor(Gdk::Cursor::create(Gdk::CursorType::SB_H_DOUBLE_ARROW));
            }
            else if(rel_pos.x > width / 2 - 5 && rel_pos.x < width / 2 + 5) {
                this->drawing_area->get_window()->set_cursor(Gdk::Cursor::create(Gdk::CursorType::SB_V_DOUBLE_ARROW));
            }
            else {
                this->drawing_area->get_window()->set_cursor(Gdk::Cursor::create(Gdk::CursorType::FLEUR));
            }
        }
        else {
            // coordinate system is being scaled/moved by user, update UI

            this->drag_update(motion->x, motion->y);
        }

        return false;
    });

    this->drawing_area->signal_button_press_event().connect([this] (GdkEventButton *button) {
        int height = this->image.pixbuf->get_height();
        int width = this->image.pixbuf->get_width();

        Coord rel_pos = Coord(button->x + this->grid_offset.x, button->y + this->grid_offset.y);

        if(button->button == BUTTONCODE_MOUSE_LEFT) {
            // user started dragging on coordinate system
            this->drag_start = rel_pos;

            if(rel_pos.y > height / 2 - 20 && rel_pos.y < height / 2 + 20 && rel_pos.x > width / 2 - 20 && rel_pos.x < width / 2 + 20) {
                this->drag_event_id = DRAG_EVENT::MOVE;
            }
            else if(rel_pos.y > height / 2 - 5 && rel_pos.y < height / 2 + 5) {
                this->drag_event_id = DRAG_EVENT::SCALE_X;
                this->drag_start_scale = this->scale.x;
            }
            else if(rel_pos.x > width / 2 - 5 && rel_pos.x < width / 2 + 5) {
                this->drag_event_id = DRAG_EVENT::SCALE_Y;
                this->drag_start_scale = this->scale.y;
            }
            else {
                this->drag_event_id = DRAG_EVENT::MOVE;
            }
        }

        return false;
    });

    this->drawing_area->signal_button_release_event().connect([this] (GdkEventButton *button) {
        if(button->button == BUTTONCODE_MOUSE_LEFT) {
            // user finished scaling coordinate system

            this->drag_update(button->x, button->y);
            this->drag_event_id = DRAG_EVENT::NONE;
        }
        return false;
    });
}

GUI::~GUI() {}

void GUI::redraw() {
    if(!this->working) {
        this->working = true;
        int width  = this->image.pixbuf->get_width();
        int height = this->image.pixbuf->get_height();
        std::thread draw_thread([this, width, height] () {
            Glib::RefPtr<Gdk::Pixbuf> pixbuf = this->image.draw_functions(this->function_store, width, height, this->scale, this->grid_offset);

            // the idle signal is thread-safe
            Glib::signal_idle().connect([this, pixbuf] () {
                this->image.pixbuf = pixbuf;
                this->drawing_area->queue_draw();
                return false;
            });

            this->working = false;
        });
        draw_thread.detach();
    }
}

void GUI::drag_update(int x, int y) {
    long double offset;
    switch(this->drag_event_id) {
        case DRAG_EVENT::MOVE:
            this->grid_offset.x = this->drag_start.x - x;
            this->grid_offset.y = this->drag_start.y - y;
            break;
        case DRAG_EVENT::SCALE_X:
            x += this->grid_offset.x;
            offset = this->image.pixbuf->get_width() / 2;
            if((this->drag_start.x - offset > 0 && x - offset > 0) || (this->drag_start.x - offset < 0 && x - offset < 0)) {
                this->scale.x = this->drag_start_scale * ((this->drag_start.x - offset) / (x - offset));
            }
            break;
        case DRAG_EVENT::SCALE_Y:
            y += this->grid_offset.y;
            offset = this->image.pixbuf->get_height() / 2;
            if((this->drag_start.y - offset > 0 && y - offset > 0) || (this->drag_start.y - offset < 0 && y - offset < 0)) {
                this->scale.y = this->drag_start_scale * ((this->drag_start.y - offset) / (y - offset));
            }
            break;
        default:
            break;
    }
    this->redraw();
}

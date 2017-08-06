#include <array>
#include <expression.hpp>
#include <future>
#include <gtkmm.h>
#include <gui.hpp>
#include <parser.hpp>
#include <string>
#include <thread>
#include <util.hpp>

using namespace util;

Image::Image() : brush({{0, 0, 0}}) {}
Image::Image(const int width, const int height) : brush({{0, 0, 0}}) {
    this->pixbuf = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, 0, 8, width, height);
}
void Image::set_pixel(const int x, const int y, const std::array<int, 3> rgb) {
    if(x >= 0 && x < this->pixbuf->get_width() && y >= 0 && y < this->pixbuf->get_height()) {
        int     offset     = y * this->pixbuf->get_rowstride() + x * this->pixbuf->get_n_channels();
        guint8 *pixels     = this->pixbuf->get_pixels();
        pixels[offset]     = rgb[0];
        pixels[offset + 1] = rgb[1];
        pixels[offset + 2] = rgb[2];
    }
}
void Image::set_pixel(const int x, const int y) {
    this->set_pixel(x, y, brush);
}
void Image::draw_char(const char c, const int x, const int y) {
    for(int bit_y = 0; bit_y < char_map[c].size(); ++bit_y) {
        for(int bit_x = 0; bit_x < char_map[c][bit_y].size(); ++bit_x) {
            if(char_map[c][bit_y][bit_x]) {
                this->set_pixel(x + bit_x, y + bit_y);
            }
        }
    }
}
void Image::draw_string(const Glib::ustring s, const long double x, const long double y) {
    int margin = 0;
    for(int i = 0; i < s.size(); ++i) {
        margin += char_map[s[i]][0].size() + 1 ;
        this->draw_char(s[i], x + margin + (s[i] == '.' ? 2 : 0), y);
    }
}
void Image::flip(const bool horizontally) {
    this->pixbuf = this->pixbuf->flip(horizontally);
}
void Image::clear() {
    int x, y;
    for(x = 0; x < this->pixbuf->get_width(); ++x) {
        for(y = 0; y < this->pixbuf->get_height(); ++y) {
            this->set_pixel(x, y, {{255, 255, 255}});
        }
    }
}
Glib::RefPtr<Gdk::Pixbuf> Image::draw_functions(Glib::RefPtr<Gtk::ListStore> function_store, const int width, const int height, const long double scale_x, const long double scale_y) {
    int                      x, y, x_iter, y_iter, margin, i, i2, diff;
    Operand*                 function;
    Image                    image(width, height);
    Glib::ustring            label;
    FunctionColumns          function_columns;
    Gtk::TreeModel::Children functions = function_store->children();

    image.clear();
    for(x_iter = 0; x_iter < width; ++x_iter) {
        x = x_iter - width / 2;
        for(y_iter = 0; y_iter < height; ++y_iter) {
            y = y_iter - height / 2;

            // draw diagram
            if(x % 30 == 0 && (y > -3 && y < 3)) {
                image.set_pixel(x_iter, y_iter);
                if(y == 0 && x != 0) {
                    label = std::to_string(scale_x * ((long double)x));
                    label = label.substr(0, label.find(".") + 3);
                    margin = label.size() * 4 - 8;
                    image.draw_string(label, x_iter - margin, y_iter + 4);
                }
            }
            else if(y % 30 == 0 && (x > -3 && x < 3)) {
                image.set_pixel(x_iter, y_iter);
                if(y != 0 && x == 0) {
                    label = std::to_string(scale_y * ((long double)y));
                    label = label.substr(0, label.find(".") + 3);
                    image.draw_string(label, x_iter - label.size() * 4 - 4, height - y_iter - 2);
                }
            }
            else if(!y || !x) {
                image.set_pixel(x_iter, y_iter);
            }
        }
    }

    std::vector<std::thread> threads;
    std::vector<std::future<std::vector<int>>> futures;

    for(const Gtk::TreeRow row : functions) {
        function = row[function_columns.function];
        std::promise<std::vector<int>> promise;
        futures.push_back(promise.get_future());
        threads.push_back(std::thread([function, width, scale_y, scale_x] (std::promise<std::vector<int>> &&promise) {
            int x, y;
            std::vector<int> values;
            for(x = 0; x < width; ++x) {
                y = function->calc(scale_x * ((long double)(x - width / 2))) / scale_y;
                values.push_back(y);
            }
            promise.set_value(values);
        }, std::move(promise)));
    }

    std::vector<int> values;
    for(i = 0; i < threads.size(); ++i) {
        threads[i].join();
        values = futures[i].get();
        for(x_iter = 0; x_iter < width; ++x_iter) {
            image.set_pixel(x_iter, height / 2 - values[x_iter]);
            if(x_iter && (std::abs(values[x_iter - 1]) <= height / 2 || std::abs(values[x_iter]) <= height / 2)) {
                diff = values[x_iter] - values[x_iter - 1];
                if(std::abs(diff) > height) {
                    diff = sign(diff) * height;
                }
                if(std::abs(diff) > 1) {
                    for(i2 = 0; i2 < std::floor(std::abs(diff) / 2); ++i2) {
                        image.set_pixel(x_iter - 1, height / 2 - values[x_iter - 1] - sign(diff) * (i2 + 1));
                        image.set_pixel(x_iter, height / 2 - values[x_iter] + sign(diff) * (i2 + (std::abs(diff) % 2 * 1)));
                    }
                }
            }
        }
    }

    return image.pixbuf;
}

FunctionColumns::FunctionColumns() {
    this->add(this->name);
    this->add(this->text);
    this->add(this->function);
    this->add(this->zero);
}

GUI::GUI() : x_scale(1.0L / 30.0L), y_scale(1.0L / 30.0L), next_name(102), drag_start(0), drag_start_scale(1), dragging_x(false), dragging_y(false), working(false) {
    /**
     * initialize GUI elements
     */
    this->builder = Gtk::Builder::create_from_resource("/org/gtk/calc/calc.glade");
    this->builder->get_widget("window_root", this->window_root);

    Glib::RefPtr<Gtk::CssProvider> css = Gtk::CssProvider::create();
    css->load_from_resource("/org/gtk/calc/calc.css");
    Glib::RefPtr<Gtk::StyleContext> style = Gtk::StyleContext::create();
    style->add_provider_for_screen(this->window_root->get_screen(), css, GTK_STYLE_PROVIDER_PRIORITY_USER);

    this->builder->get_widget("entry_function", this->entry_function);
    this->builder->get_widget("drawing_area", this->drawing_area);
    this->builder->get_widget("text_view_results", this->text_view_results);
    this->builder->get_widget("tree_view_results", this->tree_view_results);
    this->function_store = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(this->builder->get_object("function_store"));
    this->tree_view_results->set_model(this->function_store);
    this->builder->get_widget("button_derive", this->button_derive);
    this->builder->get_widget("button_delete", this->button_delete);

    // setup event handlers
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

    this->drawing_area->signal_size_allocate().connect([this](Gdk::Rectangle allocation) {
        this->image.pixbuf = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, 0, 8, allocation.get_width(), allocation.get_height());
        this->redraw();
    });

    this->drawing_area->signal_draw().connect([this](const Cairo::RefPtr<Cairo::Context> &context) {
        Gdk::Cairo::set_source_pixbuf(context, this->image.pixbuf, 0, 0);
        context->rectangle(0, 0, this->image.pixbuf->get_width(), this->image.pixbuf->get_height());
        context->fill();
        return true;
    });

    this->drawing_area->add_events(Gdk::EventMask::POINTER_MOTION_MASK | Gdk::EventMask::BUTTON_PRESS_MASK | Gdk::EventMask::BUTTON_RELEASE_MASK);
    this->drawing_area->signal_motion_notify_event().connect([this] (GdkEventMotion *motion) {
        if(!(this->dragging_x || this->dragging_y)) {
            int height = this->image.pixbuf->get_height();
            int width = this->image.pixbuf->get_width();

            if(motion->y > height / 2 - 20 && motion->y < height / 2 + 20 && motion->x > width / 2 - 20 && motion->x < width / 2 + 20) {
                this->drawing_area->get_window()->set_cursor(Gdk::Cursor::create(Gdk::CursorType::LEFT_PTR));
            }
            else if(motion->y > height / 2 - 5 && motion->y < height / 2 + 5) {
                this->drawing_area->get_window()->set_cursor(Gdk::Cursor::create(Gdk::CursorType::SB_H_DOUBLE_ARROW));
            }
            else if(motion->x > width / 2 - 5 && motion->x < width / 2 + 5) {
                this->drawing_area->get_window()->set_cursor(Gdk::Cursor::create(Gdk::CursorType::SB_V_DOUBLE_ARROW));
            }
            else {
                this->drawing_area->get_window()->set_cursor(Gdk::Cursor::create(Gdk::CursorType::LEFT_PTR));
            }
        }
        else {
            this->drag_update(motion->x, motion->y);
        }

        return false;
    });

    this->drawing_area->signal_button_press_event().connect([this] (GdkEventButton *button) {
        int height = this->image.pixbuf->get_height();
        int width = this->image.pixbuf->get_width();

        if(button->button == BUTTONCODE_MOUSE_LEFT && !(button->y > height / 2 - 20 && button->y < height / 2 + 20 && button->x > width / 2 - 20 && button->x < width / 2 + 20)) {
            if(button->y > height / 2 - 5 && button->y < height / 2 + 5) {
                this->dragging_x = true;
                this->drag_start = button->x;
                this->drag_start_scale = this->x_scale;
            }
            else if(button->x > width / 2 - 5 && button->x < width / 2 + 5) {
                this->dragging_y = true;
                this->drag_start = button->y;
                this->drag_start_scale = this->y_scale;
            }
        }
        return false;
    });

    this->drawing_area->signal_button_release_event().connect([this] (GdkEventButton *button) {
        if(button->button == BUTTONCODE_MOUSE_LEFT && (this->dragging_x || this->dragging_y)) {
            this->drag_update(button->x, button->y);
            this->dragging_x = false;
            this->dragging_y = false;
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
            Glib::RefPtr<Gdk::Pixbuf> pixbuf = this->image.draw_functions(this->function_store, width, height, this->x_scale, this->y_scale);

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
    double long pos = this->dragging_x ? x : y;
    double long offset = this->dragging_x ? this->image.pixbuf->get_width() / 2 : this->image.pixbuf->get_height() / 2;
    if((this->drag_start - offset > 0 && pos - offset > 0) || (this->drag_start - offset < 0 && pos - offset < 0)) {
        double long ratio = this->drag_start_scale * ((this->drag_start - offset) / (pos - offset));
        if(this->dragging_x) {
            this->x_scale = ratio;
        }
        else {
            this->y_scale = ratio;
        }
        this->redraw();
    }
}

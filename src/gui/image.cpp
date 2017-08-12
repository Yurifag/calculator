#include <array>
#include <expression.hpp>
#include <future>
#include <gtkmm.h>
#include <gui/image.hpp>
#include <gui/model/function_model.hpp>
#include <thread>
#include <util.hpp>
#include <vector>

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
Glib::RefPtr<Gdk::Pixbuf> Image::draw_functions(Glib::RefPtr<Gtk::ListStore> function_store, const int width, const int height, const Coord scale, const Coord offset) {
    int                      x, y, x_iter, y_iter, margin, i, i2, diff;
    Operand*                 function;
    Image                    image(width, height);
    Glib::ustring            label;
    FunctionColumns          function_columns;
    Gtk::TreeModel::Children functions = function_store->children();

    image.clear();
    for(x_iter = 0; x_iter < width; ++x_iter) {
        x = x_iter - width / 2 + ((int)offset.x);
        for(y_iter = 0; y_iter < height; ++y_iter) {
            y = y_iter - height / 2 + ((int)offset.y);

            // draw diagram
            if(x % 30 == 0 && y > -3 && y < 3) {
                // x-axis
                image.set_pixel(x_iter, y_iter);
                if(y == 0 && x != 0) {
                    label = std::to_string(scale.x * ((long double)x));
                    label = label.substr(0, label.find(".") + 3);
                    margin = label.size() * 4 - 8;
                    image.draw_string(label, x_iter - margin, y_iter + 4);
                }
            }
            else if(y % 30 == 0 && (x > -3 && x < 3)) {
                // y-axis
                image.set_pixel(x_iter, y_iter);
                if(y != 0 && x == 0) {
                    label = std::to_string(scale.y * ((long double) -y));
                    label = label.substr(0, label.find(".") + 3);
                    image.draw_string(label, x_iter - label.size() * 4 - 5, y_iter - 2);
                }
            }
            else if(!y || !x) {
                image.set_pixel(x_iter, y_iter);
            }
        }
    }

    std::vector<std::thread> threads;
    std::vector<std::future<std::vector<int>>> futures;

    // spawn a thread for each function
    for(const Gtk::TreeRow row : functions) {
        function = row[function_columns.function];
        std::promise<std::vector<int>> promise;
        futures.push_back(promise.get_future());
        threads.push_back(std::thread([function, width, scale, offset] (std::promise<std::vector<int>> &&promise) {
            int x, y;
            std::vector<int> values;
            for(x = 0; x < width; ++x) {
                y = function->calc(scale.x * ((long double)(x + offset.x - width / 2))) / scale.y;
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
            // set pixel at calculated position
            image.set_pixel(x_iter, height / 2 - values[x_iter] - offset.y);

            if(x_iter && (std::abs(values[x_iter - 1]  + offset.y) <= height / 2 || std::abs(values[x_iter] + offset.y) <= height / 2)) {
                // interpolate slope
                diff = values[x_iter] - values[x_iter - 1];
                if(std::abs(diff) > height) {
                    // height difference larger than canvas
                    diff = sign(diff) * height;
                }
                if(std::abs(diff) > 1) {
                    for(i2 = 0; i2 < std::floor(std::abs(diff) / 2); ++i2) {
                        image.set_pixel(x_iter - 1, height / 2 - values[x_iter - 1] - sign(diff) * (i2 + 1) - offset.y);
                        image.set_pixel(x_iter, height / 2 - values[x_iter] + sign(diff) * (i2 + (std::abs(diff) % 2 * 1)) - offset.y);
                    }
                }
            }
        }
    }

    return image.pixbuf;
}

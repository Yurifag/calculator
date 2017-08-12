#pragma once
#include <expression.hpp>
#include <gtkmm.h>

class FunctionColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<Glib::ustring> text;
    Gtk::TreeModelColumn<Operand*>      function;
    Gtk::TreeModelColumn<double>        zero;

    FunctionColumns();
};

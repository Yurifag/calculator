#include <gui/model/function_model.hpp>

FunctionColumns::FunctionColumns() {
    this->add(this->name);
    this->add(this->text);
    this->add(this->function);
    this->add(this->zero);
}

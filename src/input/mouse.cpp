#include "mouse.h"

#include "common/sc_assert.h"

namespace scythe {
    
    Mouse::Mouse()
    : x_(0.0f)
    , y_(0.0f)
    , delta_x_(0.0f)
    , delta_y_(0.0f)
    {
        button_down_table_[MouseButton::kUnknown] = false;
        button_down_table_[MouseButton::kLeft] = false;
        button_down_table_[MouseButton::kMiddle] = false;
        button_down_table_[MouseButton::kRight] = false;
        SC_ASSERT(button_down_table_.size() == (size_t)MouseButton::kCount);
    }
    Mouse::~Mouse()
    {
    }
    float& Mouse::x()
    {
        return x_;
    }
    float& Mouse::y()
    {
        return y_;
    }
    float& Mouse::delta_x()
    {
        return delta_x_;
    }
    float& Mouse::delta_y()
    {
        return delta_y_;
    }
    bool& Mouse::button_down(MouseButton button)
    {
        return button_down_table_[button];
    }
    
}
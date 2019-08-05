#include "concord_cursor.hh"

using namespace concord;

cursor::cursor()
  : motion{ handle_->events.motion }, motion_absolute{ handle_->events.motion_absolute },
    button{ handle_->events.button }, axis{ handle_->events.axis }, frame{ handle_->events.frame }
{
}

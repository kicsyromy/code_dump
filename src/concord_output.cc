#include "concord_output.hh"

#include "concord_server.hh"

using namespace concord;

output_layout::output_layout(server &concord_server)
  : server_{ concord_server }
  , new_output{ server_.backend->events.new_output }
{
}

void output_layout::on_new_output_added(wlr_output &output)
{
}
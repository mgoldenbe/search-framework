#ifndef SLB_EXTENSIONS_EVENTS_HEADERS_H
#define SLB_EXTENSIONS_EVENTS_HEADERS_H

/// \file
/// \brief Implementation of events generated by the search algorithms.
/// \author Meir Goldenberg

namespace slb {
namespace ext {

/// \namespace slb::ext::event
/// Events generated by the search algorithms.
namespace event {
using core::ui::EventBase;
using core::ui::EventType;
using core::ui::CurrentStyles;
using core::ui::VertexStyle;
using core::ui::EdgeStyle;
using core::util::Color;
}

} // namespace
} // namespace

#include "event_types.h"
#include "events.h"

#endif

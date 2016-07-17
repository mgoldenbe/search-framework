#ifndef PANCAKE_IMPL_H
#define PANCAKE_IMPL_H

/// \file
/// \brief Dependent implementations taken out from \ref pancake.h.
/// \author Meir Goldenberg

namespace Domains {

Pancake::Pancake() : pancakes_(CMD.nPancakes()) {
    int i = -1;
    for (auto &el: pancakes_) el = ++i;
}

}

#endif

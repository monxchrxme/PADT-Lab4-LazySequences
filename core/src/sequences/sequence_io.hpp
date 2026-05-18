#pragma once

#include <iostream>
#include "interfaces/sequence.hpp"

// free function
// it is not a class method, nor is it a friend.
// it simply gets std::ostream и Sequence<T> and do it work
template <class T>
std::ostream& operator<<(std::ostream& os, const Sequence<T>& seq) {
    os << "[";
    bool first = true;

    IEnumerator<T>* it = seq.get_enumerator();

    while (it->move_next()) {
        if (!first) {
            os << ", ";
        }
        os << it->get_current();
        first = false;
    }
    os << "]";

    delete it;

    return os;
}
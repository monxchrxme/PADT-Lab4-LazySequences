#pragma once

template <class T, class U>
struct Pair {
    T first;
    U second;

    Pair() : first(T()), second(U()) {}

    Pair(const T& f, const U& s) : first(f), second(s) {}

    bool operator==(const Pair<T, U>& other) const {
        return (first == other.first) && (second == other.second);
    }
};

template <class T, class U>
Pair<T, U> MakePair(const T& f, const U& s) {
    return Pair<T, U>(f, s);
}

template <class T, class U>
std::ostream& operator<<(std::ostream& os, const Pair<T, U>& p) {
    os << "(" << p.first << ", " << p.second << ")";
    return os;
}
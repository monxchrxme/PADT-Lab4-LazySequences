#pragma once
#include "../interfaces/sequence.hpp"
#include "mutable_array_sequence.hpp"
#include "../types/pair.hpp"

template <class T>
Sequence<Sequence<T>*>* Split(const Sequence<T>* seq, bool (*condition)(const T&)) {
    if (!seq) return nullptr;

    auto result = new MutableArraySequence<Sequence<T>*>();
    ISequenceBuilder<T>* current_chunk = seq->create_builder();

    IEnumerator<T>* it = seq->get_enumerator();
    while (it->move_next()) {
        const auto& item = it->get_current();

        if (condition(item)) {
            result->append(current_chunk->build());
            delete current_chunk;
            current_chunk = seq->create_builder();
        } else {
            current_chunk->append(item);
        }
    }
    delete it;

    result->append(current_chunk->build());
    delete current_chunk;

    return result;
}

template <class T, class U>
Sequence<Pair<T, U>>* Zip(const Sequence<T>* seq1, const Sequence<U>* seq2) {
    if (!seq1 || !seq2) throw std::invalid_argument("Zip: sequences cannot be null");

    auto builder = new typename MutableArraySequence<Pair<T, U>>::Builder();

    int len1 = seq1->get_length();
    int len2 = seq2->get_length();
    int min_len = (len1 < len2) ? len1 : len2;

    for (int i = 0; i < min_len; ++i) {
        builder->append(MakePair(seq1->get(i), seq2->get(i)));
    }

    Sequence<Pair<T, U>>* result = builder->build();
    delete builder;
    return result;
}

template <class T, class U>
Pair<Sequence<T>*, Sequence<U>*> Unzip(const Sequence<Pair<T, U>>* seq) {
    if (!seq) throw std::invalid_argument("Unzip: sequence cannot be null");

    auto builder1 = new typename MutableArraySequence<T>::Builder();
    auto builder2 = new typename MutableArraySequence<U>::Builder();

    IEnumerator<Pair<T, U>>* it = seq->get_enumerator();
    while (it->move_next()) {
        const auto& p = it->get_current();

        builder1->append(p.first);
        builder2->append(p.second);
    }
    delete it;

    Pair<Sequence<T>*, Sequence<U>*> result(builder1->build(), builder2->build());

    delete builder1;
    delete builder2;

    return result;
}

template <class T, class U>
Sequence<U>* Map(const Sequence<T>* seq, U (*mapper)(const T&)) {
    if (!seq) {
        throw std::invalid_argument("Map: sequence cannot be null");
    }

    auto builder = new typename MutableArraySequence<U>::Builder();

    IEnumerator<T>* it = seq->get_enumerator();
    while (it->move_next()) {
        builder->append(mapper(it->get_current()));
    }
    delete it;

    Sequence<U>* result = builder->build();
    delete builder;

    return result;
}
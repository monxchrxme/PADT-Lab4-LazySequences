#pragma once

#include "../interfaces/sequence.hpp"
#include "../memory/linked_list.hpp"
#include <stdexcept>

template <class T>
class ListSequence : public Sequence<T> {
protected:
    LinkedList<T> *items; // protected so that heirs can copy it

    // method that determines which object to mutate
    virtual ListSequence<T>* get_instance() = 0;

public:
    // constructors
    ListSequence();
    explicit ListSequence(int size);
    ListSequence(T *items, int count);
    ListSequence(const ListSequence<T> &other);
    ListSequence(ListSequence<T> &&other) noexcept;
    // cross-constructor from any collection
    explicit ListSequence(const ICollection<T> &collection);

    ~ListSequence() override;

    // iterators
    IEnumerator<T>* get_enumerator() const override;

    class CppIterator {
    private:
        typename LinkedList<T>::Node* current_node;

    public:
        explicit CppIterator(typename LinkedList<T>::Node* node) : current_node(node) {}

        const T& operator*() const {
            return current_node->data;
        }

        // go to next node by pointer next, not by get(index)
        CppIterator& operator++() {
            current_node = current_node->next;
            return *this;
        }

        bool operator!=(const CppIterator& other) const {
            return current_node != other.current_node;
        }
    };

    // method for using for (auto x : list_seq)
    CppIterator begin() const {
        return CppIterator(this->items->get_head_node());
    }
    CppIterator end() const {
        return CppIterator(nullptr);
    }

    // operators=
    // copy assignment operator
    ListSequence<T>& operator=(const ListSequence<T> &other);
    // move assignment operator
    ListSequence<T>& operator=(ListSequence<T> &&other) noexcept;

    // fabric methods
    Sequence<T>* create_empty() const override = 0;
    Sequence<T>* clone() const override = 0;

    // getters
    const T& get(int index) const override;
    [[nodiscard]] int get_length() const override;
    // Enumerator requires
    LinkedList<T>* get_internal_list() const {
        return items;
    }


    // modifying operations
    Sequence<T>* append(const T &item) override;
    Sequence<T>* prepend(const T &item) override;
    Sequence<T>* insert_at(const T &item, int index) override;
    Sequence<T>* remove_at(int index) override;
};

#include "list_sequence.tpp"
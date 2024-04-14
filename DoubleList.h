#pragma once

#include <cstddef>
#include <cassert>
#include <iostream>
#include <utility>

template <typename Item>
class double_linked_list
{
public:
    using       value_type      = Item;
    using       size_type       = size_t;
    using       difference_type = ptrdiff_t;
    using       pointer         = value_type *;
    using       const_pointer   = const value_type *;
    using       reference       = value_type &;
    using       const_reference = const value_type &;

private:
    struct node {
        node *next = nullptr;
        node *prev = nullptr;
        value_type data;

        node(value_type item) noexcept : data {std::move(item)} {}
    };

public:
    class double_linked_list_const_iterator {
    private:
        friend class double_linked_list;

        explicit double_linked_list_const_iterator(const node *ptr) noexcept : m_current {ptr} {}

    public:
        using       difference_type     = double_linked_list::difference_type;
        using       value_type          = double_linked_list::value_type;
        using       pointer             = double_linked_list::const_pointer;
        using       reference           = double_linked_list::const_reference;
        using       iterator_category   = std::bidirectional_iterator_tag;

        reference operator*() const noexcept {
            assert(m_current != nullptr);
            return m_current->data;
        }

        double_linked_list_const_iterator& operator++() noexcept {
            assert(m_current != nullptr);
            m_current = m_current->next;
            return *this;
        }

        double_linked_list_const_iterator& operator--() noexcept {
            assert(m_current != nullptr);
            m_current = m_current->prev;
            return *this;
        }

        double_linked_list_const_iterator operator++(int) noexcept {
            assert(m_current != nullptr);
            auto copy = *this;
            
            m_current = m_current->next;
            return copy;
        }

        double_linked_list_const_iterator operator--(int) noexcept {
            assert(m_current != nullptr);
            auto copy = *this;
            
            m_current = m_current->prev;
            return copy;
        }

        bool operator==(double_linked_list_const_iterator other) const noexcept {
            return m_current == other.m_current;
        }

        bool operator!=(double_linked_list_const_iterator other) const noexcept {
            return !(*this == other);
        }

    protected:
        const node *Get() const noexcept {
            return m_current;
        }

    protected:
        const node *m_current;
    };

    class double_linked_list_iterator : public double_linked_list_const_iterator {
    private:
        friend class double_linked_list;

        explicit double_linked_list_iterator(node *ptr) noexcept : double_linked_list_const_iterator {ptr} {}

    public:
        using       difference_type     = double_linked_list::difference_type;
        using       value_type          = double_linked_list::value_type;
        using       pointer             = double_linked_list::pointer;
        using       reference           = double_linked_list::reference;
        using       iterator_category   = std::bidirectional_iterator_tag;

        reference operator*() noexcept {
            auto &&res = double_linked_list_const_iterator::operator*();
            return const_cast<reference>(res);
        }

        double_linked_list_iterator& operator++() noexcept {
            double_linked_list_iterator::operator++();
            return *this;
        }

        double_linked_list_iterator operator++(int) noexcept {
            auto res = double_linked_list_const_iterator::operator++(0);
            return double_linked_list_iterator {const_cast<node *>(res.Get())};
        }

        double_linked_list_iterator& operator--() noexcept {
            double_linked_list_iterator::operator--();
            return *this;
        }

        double_linked_list_iterator operator--(int) noexcept {
            auto res = double_linked_list_const_iterator::operator--(0);
            return double_linked_list_iterator {const_cast<node *>(res.Get())};
        }
    };

    using       iterator       = double_linked_list_iterator;
    using       const_iterator = double_linked_list_const_iterator;

    double_linked_list() = default;

    double_linked_list(std::initializer_list<value_type> items) {
        for (auto &item : items) {
            push_back(item);
        }
    }

    ~double_linked_list() {
        clear();
    }

    void clear() noexcept {
        while (m_head) {
            delete std::exchange(m_head, m_head->next);
        }

        m_tail = nullptr;
    }

    void push_front(value_type item) {
        auto newnode = new node {std::move(item)};
        if (m_head) {
            m_head->prev = newnode;
            newnode->next = m_head;
            m_head = newnode;
        } else {
            m_head = m_tail = newnode;
        }
    }

    void push_back(value_type item) {
        auto newnode = new node {std::move(item)};
        if (m_tail) {
            m_tail->next = newnode;
            newnode->prev = m_tail;
            m_tail = newnode;
        } else {
            m_head = m_tail = newnode;
        }
    }

    void insert(const_iterator place, value_type item) {
        auto ptr = const_cast<node *>(place.Get());
        if (!ptr) {
            push_back(std::move(item));
            return;
        }

        auto newnode = new node {std::move(item)};
    
        newnode->next = ptr;
        newnode->prev = ptr->prev;

        if (ptr->prev) {
            ptr->prev->next = newnode;
        }
    
        ptr->prev = newnode;
    }

    const_iterator find(const_reference item) const noexcept {
        for (auto it = begin(); it != end(); ++it) {
            if (*it == item) {
                return it;
            }
        }

        return const_iterator {nullptr};
    }

    iterator find(const_reference item) noexcept {
        auto it = static_cast<const double_linked_list &>(*this).find(item);
        return iterator {const_cast<node *>(it.Get())};
    }

    void erase(const_iterator place) noexcept {
        auto ptr = const_cast<node *>(place.Get());
        assert(ptr != nullptr);

        if (ptr->prev) {
            ptr->prev->next = ptr->next;
        } else {
            m_head = ptr->next;
        }

        if (ptr->next) {
            ptr->next->prev = ptr->prev;
        } else {
            m_tail = ptr->prev;
        }

        delete ptr;
    }

    void pop_front() noexcept {
        erase(begin());
    }
  
    const_iterator begin() const noexcept {
        return const_iterator {m_head};
    }

    const_iterator end() const noexcept {
        return const_iterator {nullptr};
    }

    const_iterator cbegin() const noexcept {
        return const_iterator {m_head};
    }

    const_iterator cend() const noexcept {
        return const_iterator {nullptr};
    }

    iterator begin() noexcept {
        return iterator {m_head};
    }

    iterator end() noexcept {
        return iterator {nullptr};
    }

private:
    node *m_head = nullptr;
    node *m_tail = nullptr;
};

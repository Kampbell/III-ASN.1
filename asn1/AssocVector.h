////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author or Addison-Wesley Longman make no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////
#ifndef ASSOCVECTOR_INC_
#define ASSOCVECTOR_INC_

#include <algorithm>
#include <functional>
#include <vector>
#include <utility>

namespace ASN1
{
////////////////////////////////////////////////////////////////////////////////
// class template AssocVectorCompare
// Used by AssocVector
////////////////////////////////////////////////////////////////////////////////

    namespace Private
    {
        template <class Value, class C>
        class AssocVectorCompare : public C
        {
            typedef ASN1_STD pair<typename C::first_argument_type, Value>
                Data;
            typedef typename C::first_argument_type first_argument_type;

        public:
            AssocVectorCompare()
            {}

            AssocVectorCompare(const C& src) : C(src)
            {}

            bool operator()(const first_argument_type& lhs,
                const first_argument_type& rhs) const
            { return C::operator()(lhs, rhs); }

            bool operator()(const Data& lhs, const Data& rhs) const
            { return operator()(lhs.first, rhs.first); }

            bool operator()(const Data& lhs,
                const first_argument_type& rhs) const
            { return operator()(lhs.first, rhs); }

            bool operator()(const first_argument_type& lhs,
                const Data& rhs) const
            { return operator()(lhs, rhs.first); }
        };
    }

////////////////////////////////////////////////////////////////////////////////
// class template AssocVector
// An associative vector built as a syntactic drop-in replacement for std::map
// BEWARE: AssocVector doesn't respect all map's guarantees, the most important
//     being:
// * iterators are invalidated by insert and erase operations
// * the complexity of insert/erase is O(N) not O(log N)
// * value_type is std::pair<K, V> not std::pair<const K, V>
// * iterators are random
////////////////////////////////////////////////////////////////////////////////

    template
    <
        class K,
        class V,
        class C = ASN1_STD less<K>,
#if defined(HP_aCC_RW)
        class A = ASN1_STD allocator
#else
        class A = ASN1_STD allocator< ASN1_STD pair<K, V> >
#endif
    >
    class AssocVector
        : private ASN1_STD vector< ASN1_STD pair<K, V>, A >
        , private Private::AssocVectorCompare<V, C>
    {
        typedef ASN1_STD vector<ASN1_STD pair<K, V>, A> Base;
        typedef Private::AssocVectorCompare<V, C> MyCompare;

    public:
        typedef K key_type;
        typedef V mapped_type;
        typedef typename Base::value_type value_type;

        typedef C key_compare;
        typedef A allocator_type;
#if defined(HP_aCC_RW)
        typedef typename Base::reference reference;
        typedef typename Base::const_reference const_reference;
        typedef typename Base::pointer pointer;
        typedef typename Base::const_pointer const_pointer;
#else
        typedef typename A::reference reference;
        typedef typename A::const_reference const_reference;
        typedef typename A::pointer pointer;
        typedef typename A::const_pointer const_pointer;
#endif
        typedef typename Base::iterator iterator;
        typedef typename Base::const_iterator const_iterator;
        typedef typename Base::size_type size_type;
        typedef typename Base::difference_type difference_type;
        typedef typename Base::reverse_iterator reverse_iterator;
        typedef typename Base::const_reverse_iterator const_reverse_iterator;

      class value_compare
            : public ASN1_STD binary_function<value_type, value_type, bool>
            , private key_compare
        {
            friend class AssocVector;

        protected:
            value_compare(key_compare pred) : key_compare(pred)
            {}

        public:
            bool operator()(const value_type& lhs, const value_type& rhs) const
            { return key_compare::operator()(lhs.first, rhs.first); }
        };

        // 23.3.1.1 construct/copy/destroy

        AssocVector(const key_compare& comp = key_compare(),
            const A& alloc = A())
        : Base(alloc), MyCompare(comp)
        {}

        template <class InputIterator>
        AssocVector(InputIterator first, InputIterator last,
            const key_compare& comp = key_compare(),
            const A& alloc = A())
        : Base(first, last, alloc), MyCompare(comp)
        {
            MyCompare& me = *this;
            ASN1_STD sort(begin(), end(), me);
        }

        AssocVector& operator=(const AssocVector& rhs)
        {
          AssocVector(rhs).swap(*this);
          return *this;
        }

        // iterators:
        // The following are here because MWCW gets 'using' wrong
        iterator begin() { return Base::begin(); }
        const_iterator begin() const { return Base::begin(); }
        iterator end() { return Base::end(); }
        const_iterator end() const { return Base::end(); }
        reverse_iterator rbegin() { return Base::rbegin(); }
        const_reverse_iterator rbegin() const { return Base::rbegin(); }
        reverse_iterator rend() { return Base::rend(); }
        const_reverse_iterator rend() const { return Base::rend(); }

        // capacity:
        bool empty() const { return Base::empty(); }
        size_type size() const { return Base::size(); }
        size_type max_size() { return Base::max_size(); }

        // 23.3.1.2 element access:
        mapped_type& operator[](const key_type& key)
        { return insert(value_type(key, mapped_type())).first->second; }

        // modifiers:
        ASN1_STD pair<iterator, bool> insert(const value_type& val)
        {
            bool found(true);
            iterator i(lower_bound(val.first));

            if (i == end() || this->operator()(val.first, i->first))
            {
                i = Base::insert(i, val);
                found = false;
            }
            return ASN1_STD make_pair(i, !found);
        }

        iterator insert(iterator pos, const value_type& val)
        {
            if (pos != end() && this->operator()(*pos, val) &&
                (pos == end() - 1 ||
                    !this->operator()(val, pos[1]) &&
                        this->operator()(pos[1], val)))
            {
                return Base::insert(pos, val);
            }
            return insert(val).first;
        }

        template <class InputIterator>
        void insert(InputIterator first, InputIterator last)
        { for (; first != last; ++first) insert(*first); }

        void erase(iterator pos)
        { Base::erase(pos); }

        size_type erase(const key_type& k)
        {
            iterator i(find(k));
            if (i == end()) return 0;
            erase(i);
            return 1;
        }

        void erase(iterator first, iterator last)
        { Base::erase(first, last); }

        void swap(AssocVector& other)
        {
            //using namespace std;
      Base::swap(other);
            MyCompare& me = *this;
            MyCompare& rhs = other;
      ASN1_STD swap(me, rhs);
        }

        void clear()
        { Base::clear(); }

        // observers:
        key_compare key_comp() const
        { return *this; }

        value_compare value_comp() const
        {
            const key_compare& comp = *this;
            return value_compare(comp);
        }

        // 23.3.1.3 map operations:
        iterator find(const key_type& k)
        {
            iterator i(lower_bound(k));
            if (i != end() && this->operator()(k, i->first))
            {
                i = end();
            }
            return i;
        }

        const_iterator find(const key_type& k) const
        {
            const_iterator i(lower_bound(k));
            if (i != end() && this->operator()(k, i->first))
            {
                i = end();
            }
            return i;
        }

        size_type count(const key_type& k) const
        { return find(k) != end(); }

        iterator lower_bound(const key_type& k)
        {
            MyCompare& me = *this;
#if defined(HP_aCC_RW)
            return ASN1_STD __lower_bound(begin(), end(), k, me, distance_type(begin()), iterator_category(begin()));
#else
            return ASN1_STD lower_bound(begin(), end(), k, me);
#endif
        }

        const_iterator lower_bound(const key_type& k) const
        {
            const MyCompare& me = *this;
#if defined(HP_aCC_RW)
            return ASN1_STD __lower_bound(begin(), end(), k, me, distance_type(begin()), iterator_category(begin()));
#else
            return ASN1_STD lower_bound(begin(), end(), k, me);
#endif
        }

        iterator upper_bound(const key_type& k)
        {
            MyCompare& me = *this;
            return ASN1_STD upper_bound(begin(), end(), k, me);
        }

        const_iterator upper_bound(const key_type& k) const
        {
            const MyCompare& me = *this;
            return ASN1_STD upper_bound(begin(), end(), k, me);
        }

        ASN1_STD pair<iterator, iterator> equal_range(const key_type& k)
        {
            MyCompare& me = *this;
            return ASN1_STD equal_range(begin(), end(), k, me);
        }

        ASN1_STD pair<const_iterator, const_iterator> equal_range(
            const key_type& k) const
        {
            const MyCompare& me = *this;
            return ASN1_STD equal_range(begin(), end(), k, me);
        }

        bool operator==(const AssocVector& rhs) const
        {
            const Base& me = *this;
            return me == rhs;
        }

        bool operator<(const AssocVector& rhs) const
        {
            const Base& me = *this;
            return me < rhs;
        }

        bool operator!=(const AssocVector& rhs) const
        { return !(*this == rhs); }

        bool operator>(const AssocVector& rhs) const
        { return rhs < *this; }

        bool operator>=(const AssocVector& rhs) const
        { return !(*this < rhs); }

        bool operator<=(const AssocVector& rhs) const
        { return !(rhs < *this); }
    };

    // specialized algorithms:
    template <class K, class V, class C, class A>
    void swap(AssocVector<K, V, C, A>& lhs, AssocVector<K, V, C, A>& rhs)
    { lhs.swap(rhs); }

} // namespace ASN1

#endif // ASSOCVECTOR_INC_

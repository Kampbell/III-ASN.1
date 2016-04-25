//
// asn1_useful.h
//
// Code automatically generated by asnparser.
//

#ifndef __ASN1_USEFUL_H
#define __ASN1_USEFUL_H

#include "asn1.h"


namespace ASN1 {

//
// EXTERNAL_encoding
//

class ASN1_API EXTERNAL_encoding : public ASN1::CHOICE
{
    typedef ASN1::CHOICE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EXTERNAL_encoding(const void* info) : Inherited(info) {}
public:
    EXTERNAL_encoding() : Inherited(&theInfo) {}
    enum Choice_Ids {
      unknownSelection_ = -2,
      unselected_ = -1,
      single_ASN1_type0 = 0,
      octet_aligned1 = 1,
      arbitrary2 = 2
    };

    class single_ASN1_type
    {
    public:
        enum Id { eid = 0 };
        typedef ASN1::OpenData value_type;

        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class single_ASN1_type

    single_ASN1_type::const_reference get_single_ASN1_type () const
    {
        assert(currentSelection() == single_ASN1_type::eid);
        return *static_cast<single_ASN1_type::const_pointer>(choice.get());
    }
    single_ASN1_type::reference ref_single_ASN1_type ()
    {
        assert(currentSelection() == single_ASN1_type::eid);
        return *static_cast<single_ASN1_type::pointer>(choice.get());
    }
    single_ASN1_type::reference select_single_ASN1_type ()
    {
        return *static_cast<single_ASN1_type::pointer>(setSelection(single_ASN1_type::eid, ASN1::AbstractData::create( &single_ASN1_type::value_type::theInfo)));
    }
    single_ASN1_type::reference select_single_ASN1_type (single_ASN1_type::const_reference v) { return select_single_ASN1_type() = v; }
    bool single_ASN1_type_isSelected() const { return currentSelection() == single_ASN1_type::eid; }

    EXTERNAL_encoding(single_ASN1_type::Id id, single_ASN1_type::const_reference v)
       : Inherited(&theInfo, id, new single_ASN1_type::value_type(v)) {}

    class octet_aligned
    {
    public:
        enum Id { eid = 1 };
        typedef ASN1::OCTET_STRING value_type;

        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class octet_aligned

    octet_aligned::const_reference get_octet_aligned () const
    {
        assert(currentSelection() == octet_aligned::eid);
        return *static_cast<octet_aligned::const_pointer>(choice.get());
    }
    octet_aligned::reference ref_octet_aligned ()
    {
        assert(currentSelection() == octet_aligned::eid);
        return *static_cast<octet_aligned::pointer>(choice.get());
    }
    octet_aligned::reference select_octet_aligned ()
    {
        return *static_cast<octet_aligned::pointer>(setSelection(octet_aligned::eid, ASN1::AbstractData::create( &octet_aligned::value_type::theInfo)));
    }
    octet_aligned::reference select_octet_aligned (const octets& v) { return select_octet_aligned() = v; }
    bool octet_aligned_isSelected() const { return currentSelection() == octet_aligned::eid; }

    EXTERNAL_encoding(octet_aligned::Id id, const octets& v)
       : Inherited(&theInfo, id, new octet_aligned::value_type(v)) {}

    class arbitrary
    {
    public:
        enum Id { eid = 2 };
        typedef ASN1::BIT_STRING value_type;

        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class arbitrary

    arbitrary::const_reference get_arbitrary () const
    {
        assert(currentSelection() == arbitrary::eid);
        return *static_cast<arbitrary::const_pointer>(choice.get());
    }
    arbitrary::reference ref_arbitrary ()
    {
        assert(currentSelection() == arbitrary::eid);
        return *static_cast<arbitrary::pointer>(choice.get());
    }
    arbitrary::reference select_arbitrary ()
    {
        return *static_cast<arbitrary::pointer>(setSelection(arbitrary::eid, ASN1::AbstractData::create( &arbitrary::value_type::theInfo)));
    }
    arbitrary::reference select_arbitrary (arbitrary::const_reference v) { return select_arbitrary() = v; }
    bool arbitrary_isSelected() const { return currentSelection() == arbitrary::eid; }

    EXTERNAL_encoding(arbitrary::Id id, arbitrary::const_reference v)
       : Inherited(&theInfo, id, new arbitrary::value_type(v)) {}
    EXTERNAL_encoding(const EXTERNAL_encoding & that)     : Inherited(that) {}

    EXTERNAL_encoding & operator=(const EXTERNAL_encoding & that)
    { Inherited::operator=(that); return *this; }

    void swap(EXTERNAL_encoding & that) { Inherited::swap(that); }

    EXTERNAL_encoding * clone() const
    { return static_cast<EXTERNAL_encoding*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const void* selectionInfos[3];
    static unsigned selectionTags[3];
    static const char* selectionNames[3];
}; // end class EXTERNAL_encoding

//
// EXTERNAL
//

class ASN1_API EXTERNAL : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EXTERNAL(const void* info) : Inherited(info) {}
public:
    EXTERNAL() : Inherited(&theInfo) {}
    EXTERNAL(const EXTERNAL & that) : Inherited(that) {}

    EXTERNAL& operator=(const EXTERNAL& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_direct_reference,
      e_indirect_reference,
      e_data_value_descriptor
    };

    class direct_reference
    {
    public:
        typedef ASN1::OBJECT_IDENTIFIER value_type;

        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class direct_reference

    direct_reference::const_reference get_direct_reference () const
    {
      assert(hasOptionalField(e_direct_reference));
      return *static_cast<direct_reference::const_pointer>(fields[0]);
    }
    direct_reference::reference ref_direct_reference ()
    {
      assert(hasOptionalField(e_direct_reference));
      return *static_cast<direct_reference::pointer>(fields[0]);
    }
    direct_reference::reference set_direct_reference ()
    {
      includeOptionalField( e_direct_reference, 0);
      return *static_cast<direct_reference::pointer>(fields[0]);
    }
    direct_reference::reference set_direct_reference (direct_reference::const_reference v)
    {
      includeOptionalField( e_direct_reference, 0);
      return *static_cast<direct_reference::pointer>(fields[0]) = v;
    }
    void omit_direct_reference () { removeOptionalField( e_direct_reference); }
    bool direct_reference_isPresent () const { return hasOptionalField( e_direct_reference); }

    class indirect_reference
    {
    public:
        typedef ASN1::INTEGER value_type;

        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class indirect_reference

    indirect_reference::const_reference get_indirect_reference () const
    {
      assert(hasOptionalField(e_indirect_reference));
      return *static_cast<indirect_reference::const_pointer>(fields[1]);
    }
    indirect_reference::reference ref_indirect_reference ()
    {
      assert(hasOptionalField(e_indirect_reference));
      return *static_cast<indirect_reference::pointer>(fields[1]);
    }
    indirect_reference::reference set_indirect_reference ()
    {
      includeOptionalField( e_indirect_reference, 1);
      return *static_cast<indirect_reference::pointer>(fields[1]);
    }
    indirect_reference::reference set_indirect_reference (indirect_reference::value_type::int_type v)
    {
      includeOptionalField( e_indirect_reference, 1);
      return *static_cast<indirect_reference::pointer>(fields[1]) = v;
    }
    void omit_indirect_reference () { removeOptionalField( e_indirect_reference); }
    bool indirect_reference_isPresent () const { return hasOptionalField( e_indirect_reference); }

    class data_value_descriptor
    {
    public:
        typedef ObjectDescriptor value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class data_value_descriptor

    data_value_descriptor::const_reference get_data_value_descriptor () const
    {
      assert(hasOptionalField(e_data_value_descriptor));
      return *static_cast<data_value_descriptor::const_pointer>(fields[2]);
    }
    data_value_descriptor::reference ref_data_value_descriptor ()
    {
      assert(hasOptionalField(e_data_value_descriptor));
      return *static_cast<data_value_descriptor::pointer>(fields[2]);
    }
    data_value_descriptor::reference set_data_value_descriptor ()
    {
      includeOptionalField( e_data_value_descriptor, 2);
      return *static_cast<data_value_descriptor::pointer>(fields[2]);
    }
    data_value_descriptor::reference set_data_value_descriptor (data_value_descriptor::const_reference v)
    {
      includeOptionalField( e_data_value_descriptor, 2);
      return *static_cast<data_value_descriptor::pointer>(fields[2]) = v;
    }
    void omit_data_value_descriptor () { removeOptionalField( e_data_value_descriptor); }
    bool data_value_descriptor_isPresent () const { return hasOptionalField( e_data_value_descriptor); }

    class encoding
    {
    public:
        typedef EXTERNAL_encoding value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class encoding

    encoding::const_reference get_encoding () const { return *static_cast<encoding::const_pointer>(fields[3]); }
    encoding::reference ref_encoding () { return *static_cast<encoding::pointer>(fields[3]); }
    encoding::reference set_encoding () { return *static_cast<encoding::pointer>(fields[3]); }
    encoding::reference set_encoding (encoding::const_reference v)
    { return *static_cast<encoding::pointer>(fields[3]) = v; }
    void swap(EXTERNAL& that) { Inherited::swap(that); }
    EXTERNAL * clone() const
    { return static_cast<EXTERNAL*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[4];
    static int fieldIds[4];
    static const char* fieldNames[4];
}; // end class EXTERNAL


//
// ABSTRACT-SYNTAX
//

class ASN1_API ABSTRACT_SYNTAX
{
public:
    typedef ASN1::OBJECT_IDENTIFIER key_type;
    class info_type
    {
    public:
        info_type();
        ASN1::AbstractData* get_Type() const { return m_Type ? ASN1::AbstractData::create(m_Type) : NULL; }
    protected:
        const void* m_Type;
    };

    typedef const info_type* mapped_type;

    class value_type : public ASN1_STD pair<key_type, mapped_type>
    {
        typedef ASN1_STD pair<key_type, mapped_type> Inherited;
    public:
        value_type(const key_type& key, mapped_type mt) : Inherited(key,mt) {}
        ASN1::AbstractData* get_Type() const { return second->get_Type(); }
    };

    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef ASN1::AssocVector<key_type, mapped_type> map_type;

private:
#if defined(HP_aCC_RW)
    typedef ASN1_STD bidirectional_iterator<value_type> my_iterator_traits;
    typedef ASN1_STD bidirectional_iterator<const value_type> my_const_iterator_traits;
#else
    typedef ASN1_STD iterator<ASN1_STD bidirectional_iterator_tag, value_type> my_iterator_traits;
    typedef ASN1_STD iterator<ASN1_STD bidirectional_iterator_tag, const value_type> my_const_iterator_traits;
#endif
public:
    class iterator : public my_iterator_traits
    {
    public:
        iterator() {}
        iterator(map_type::iterator i) : itsIter(i) {}
        map_type::iterator base()   const { return itsIter; }
        const_reference operator*() const { return *static_cast<const_pointer>(&*itsIter); }
        const_pointer operator->()  const { return static_cast<const_pointer>(&*itsIter); }
        iterator& operator++()           { ++itsIter; return *this; }
        iterator& operator--()           { --itsIter; return *this; }
        iterator operator++(int)         { iterator t(*this); ++itsIter; return t; }
        iterator operator--(int)         { iterator t(*this); --itsIter; return t; }

        bool operator==(const iterator& r) const    { return itsIter == r.itsIter; }
        bool operator!=(const iterator& r) const    { return itsIter != r.itsIter; }
    private:
        map_type::iterator itsIter;
    };
    class const_iterator : public my_const_iterator_traits
    {
    public:
        const_iterator() {}
        const_iterator(ABSTRACT_SYNTAX::iterator i) : itsIter(i.base()) {}
        const_iterator(map_type::const_iterator i) : itsIter(i) {}
        map_type::const_iterator base() const { return itsIter; }

        const_reference operator*() const { return *static_cast<const_pointer>(&*itsIter); }
        const_pointer operator->() const { return static_cast<const_pointer>(&*itsIter); }

        const_iterator& operator++()          { ++itsIter; return *this; }
        const_iterator& operator--()          { --itsIter; return *this; }
        const_iterator operator++(int)        { const_iterator t(*this); ++itsIter; return t; }
        const_iterator operator--(int)        { const_iterator t(*this); --itsIter; return t; }

        bool operator==(const const_iterator& r) const    { return itsIter == r.itsIter; }
        bool operator!=(const const_iterator& r) const    { return itsIter != r.itsIter; }
    private:
        map_type::const_iterator itsIter;
    };

//#if defined(BOOST_NO_STD_ITERATOR) || defined (BOOST_MSVC_STD_ITERATOR)
//    typedef std::reverse_bidirectional_iterator<iterator, value_type> reverse_iterator;
//    typedef std::reverse_bidirectional_iterator<const_iterator, const value_type> const_reverse_iterator;
//#else
//    typedef std::reverse_iterator<iterator, value_type> reverse_iterator;
//    typedef std::reverse_iterator<const_iterator, const value_type> const_reverse_iterator;
//#endif

    typedef map_type::key_compare key_compare;
    typedef map_type::difference_type difference_type;
    typedef map_type::size_type size_type;

    ABSTRACT_SYNTAX(){}
    template <class InputIterator>
            ABSTRACT_SYNTAX(InputIterator first, InputIterator last)
        : rep(first, last) {}
    ABSTRACT_SYNTAX(const ABSTRACT_SYNTAX& that)
        : rep(that.rep) {}

    ABSTRACT_SYNTAX& operator=(const ABSTRACT_SYNTAX& that)
        { ABSTRACT_SYNTAX tmp(that); swap(tmp);  return *this; }

    // iterators
    iterator begin() { return iterator(rep.begin()); }
    const_iterator begin() const { return const_iterator(rep.begin()); }
    iterator end() { return iterator(rep.end()); }
    const_iterator end() const { return const_iterator(rep.end()); }

//    reverse_iterator rbegin() { return reverse_iterator(end()); }
//    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
//    reverse_iterator rend() { return reverse_iterator(begin()); }
//    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    // capacity
    bool empty() const { return rep.empty(); }
    size_type size() const { return rep.size(); }
    // modifiers
    ASN1_STD pair<iterator, bool> insert(const value_type& x)
    {
        ASN1_STD pair<map_type::iterator, bool> r = rep.insert(x);
        return ASN1_STD pair<iterator, bool>(r.first, r.second);
    }
    iterator insert(iterator position, const value_type& x)
    { return iterator(rep.insert(position.base(), x)); }
    void insert(const_iterator first, const_iterator last)
    { rep.insert(first.base(), last.base()); }
    void erase(iterator position) { rep.erase(position.base()); }
    void erase(const key_type& key) { rep.erase(key); }
    void erase(iterator first, iterator last) { rep.erase(first.base(), last.base()); }
    void swap(ABSTRACT_SYNTAX& that) { rep.swap(that.rep); }
    void clear() { rep.clear(); }
    key_compare key_comp() const { return rep.key_comp(); }
    // operations
    iterator find(const key_type& key) { return iterator(rep.find(key)); }
    const_iterator find(const key_type& key) const { return const_iterator(rep.find(key)); }
    size_type count(const key_type& key) const { return rep.count(key); }
private:
    map_type rep;
};

//
// TYPE-IDENTIFIER
//

class ASN1_API TYPE_IDENTIFIER
{
public:
    typedef ASN1::OBJECT_IDENTIFIER key_type;
    class info_type
    {
    public:
        info_type();
        ASN1::AbstractData* get_Type() const { return m_Type ? ASN1::AbstractData::create(m_Type) : NULL; }
    protected:
        const void* m_Type;
    };

    typedef const info_type* mapped_type;

    class value_type : public ASN1_STD pair<key_type, mapped_type>
    {
        typedef ASN1_STD pair<key_type, mapped_type> Inherited;
    public:
        value_type(const key_type& key, mapped_type mt) : Inherited(key,mt) {}
        ASN1::AbstractData* get_Type() const { return second->get_Type(); }
    };

    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef ASN1::AssocVector<key_type, mapped_type> map_type;

private:
#if defined(HP_aCC_RW)
    typedef ASN1_STD bidirectional_iterator<value_type> my_iterator_traits;
    typedef ASN1_STD bidirectional_iterator<const value_type> my_const_iterator_traits;
#else
    typedef ASN1_STD iterator<ASN1_STD bidirectional_iterator_tag, value_type> my_iterator_traits;
    typedef ASN1_STD iterator<ASN1_STD bidirectional_iterator_tag, const value_type> my_const_iterator_traits;
#endif
public:
    class iterator : public my_iterator_traits
    {
    public:
        iterator() {}
        iterator(map_type::iterator i) : itsIter(i) {}
        map_type::iterator base()   const { return itsIter; }
        const_reference operator*() const { return *static_cast<const_pointer>(&*itsIter); }
        const_pointer operator->()  const { return static_cast<const_pointer>(&*itsIter); }
        iterator& operator++()           { ++itsIter; return *this; }
        iterator& operator--()           { --itsIter; return *this; }
        iterator operator++(int)         { iterator t(*this); ++itsIter; return t; }
        iterator operator--(int)         { iterator t(*this); --itsIter; return t; }

        bool operator==(const iterator& r) const    { return itsIter == r.itsIter; }
        bool operator!=(const iterator& r) const    { return itsIter != r.itsIter; }
    private:
        map_type::iterator itsIter;
    };
    class const_iterator : public my_const_iterator_traits
    {
    public:
        const_iterator() {}
        const_iterator(TYPE_IDENTIFIER::iterator i) : itsIter(i.base()) {}
        const_iterator(map_type::const_iterator i) : itsIter(i) {}
        map_type::const_iterator base() const { return itsIter; }

        const_reference operator*() const { return *static_cast<const_pointer>(&*itsIter); }
        const_pointer operator->() const { return static_cast<const_pointer>(&*itsIter); }

        const_iterator& operator++()          { ++itsIter; return *this; }
        const_iterator& operator--()          { --itsIter; return *this; }
        const_iterator operator++(int)        { const_iterator t(*this); ++itsIter; return t; }
        const_iterator operator--(int)        { const_iterator t(*this); --itsIter; return t; }

        bool operator==(const const_iterator& r) const    { return itsIter == r.itsIter; }
        bool operator!=(const const_iterator& r) const    { return itsIter != r.itsIter; }
    private:
        map_type::const_iterator itsIter;
    };

//#if defined(BOOST_NO_STD_ITERATOR) || defined (BOOST_MSVC_STD_ITERATOR)
//    typedef std::reverse_bidirectional_iterator<iterator, value_type> reverse_iterator;
//    typedef std::reverse_bidirectional_iterator<const_iterator, const value_type> const_reverse_iterator;
//#else
//    typedef std::reverse_iterator<iterator, value_type> reverse_iterator;
//    typedef std::reverse_iterator<const_iterator, const value_type> const_reverse_iterator;
//#endif

    typedef map_type::key_compare key_compare;
    typedef map_type::difference_type difference_type;
    typedef map_type::size_type size_type;

    TYPE_IDENTIFIER(){}
    template <class InputIterator>
            TYPE_IDENTIFIER(InputIterator first, InputIterator last)
        : rep(first, last) {}
    TYPE_IDENTIFIER(const TYPE_IDENTIFIER& that)
        : rep(that.rep) {}

    TYPE_IDENTIFIER& operator=(const TYPE_IDENTIFIER& that)
        { TYPE_IDENTIFIER tmp(that); swap(tmp);  return *this; }

    // iterators
    iterator begin() { return iterator(rep.begin()); }
    const_iterator begin() const { return const_iterator(rep.begin()); }
    iterator end() { return iterator(rep.end()); }
    const_iterator end() const { return const_iterator(rep.end()); }

//    reverse_iterator rbegin() { return reverse_iterator(end()); }
//    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
//    reverse_iterator rend() { return reverse_iterator(begin()); }
//    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    // capacity
    bool empty() const { return rep.empty(); }
    size_type size() const { return rep.size(); }
    // modifiers
    ASN1_STD pair<iterator, bool> insert(const value_type& x)
    {
        ASN1_STD pair<map_type::iterator, bool> r = rep.insert(x);
        return ASN1_STD pair<iterator, bool>(r.first, r.second);
    }
    iterator insert(iterator position, const value_type& x)
    { return iterator(rep.insert(position.base(), x)); }
    void insert(const_iterator first, const_iterator last)
    { rep.insert(first.base(), last.base()); }
    void erase(iterator position) { rep.erase(position.base()); }
    void erase(const key_type& key) { rep.erase(key); }
    void erase(iterator first, iterator last) { rep.erase(first.base(), last.base()); }
    void swap(TYPE_IDENTIFIER& that) { rep.swap(that.rep); }
    void clear() { rep.clear(); }
    key_compare key_comp() const { return rep.key_comp(); }
    // operations
    iterator find(const key_type& key) { return iterator(rep.find(key)); }
    const_iterator find(const key_type& key) const { return const_iterator(rep.find(key)); }
    size_type count(const key_type& key) const { return rep.count(key); }
private:
    map_type rep;
};

} // namespace ASN1

#endif // __ASN1_USEFUL_H

// End of asn1_useful.h

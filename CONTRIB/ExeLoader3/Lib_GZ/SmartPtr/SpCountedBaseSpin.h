#ifndef GZ_SMART_PTR_DETAIL_SP_COUNTED_BASE_SPIN_HPP_INCLUDED
#define GZ_SMART_PTR_DETAIL_SP_COUNTED_BASE_SPIN_HPP_INCLUDED

//This file is part of "GZE - GroundZero Engine"

//The permisive licence allow to use GZE for free or commercial project (Apache License, Version 2.0).
//For conditions of distribution and use, see copyright notice in Licence.txt, this license must be included with any distribution of the code.

//Though not required by the license agreement, please consider the following will be greatly appreciated:
//- We would like to hear about projects where GZE is used.
//- Include an attribution statement somewhere in your project.
//- If you want to see GZE evolve please help us with a donation.

//// This file was in part from the Boost lirairie with some modification, under this licence :

//  Copyright 2002, 2009 Peter Dimov
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.Boost.org/LICENSE_1_0.txt

namespace GZptr
{

namespace detail
{

inline int atomic_exchange_and_add( int * pw, int dv )
{
   // spinlock_pool<1>::scoped_lock lock( pw );

    int r = *pw;
    *pw += dv;
    return r;
}

inline void atomic_increment( int * pw )
{
  //  spinlock_pool<1>::scoped_lock lock( pw );
    ++*pw;
}

inline int atomic_conditional_increment( int * pw )
{
 //   spinlock_pool<1>::scoped_lock lock( pw );

    int rv = *pw;
    if( rv != 0 ) ++*pw;
    return rv;
}

class sp_counted_base
{
private:

    sp_counted_base( sp_counted_base const & );
    sp_counted_base & operator= ( sp_counted_base const & );

    int use_count_;        // #shared
    int weak_count_;       // #weak + (#shared != 0)

public:

    sp_counted_base(): use_count_( 1 ), weak_count_( 1 )
    {
    }

    virtual ~sp_counted_base() // nothrow
    {
    }

    // dispose() is called when use_count_ drops to zero, to release
    // the resources managed by *this.

    virtual void dispose() = 0; // nothrow

    // destroy() is called when weak_count_ drops to zero.

    virtual void destroy() // nothrow
    {
        delete this;
    }

    virtual void * get_untyped_deleter() = 0;

    void add_ref_copy()
    {
        atomic_increment( &use_count_ );
    }

    bool add_ref_lock() // true on success
    {
        return atomic_conditional_increment( &use_count_ ) != 0;
    }

    void release() // nothrow
    {
        if( atomic_exchange_and_add( &use_count_, -1 ) == 1 )
        {
            dispose();
            weak_release();
        }
    }

    void weak_add_ref() // nothrow
    {
        atomic_increment( &weak_count_ );
    }

    void weak_release() // nothrow
    {
        if( atomic_exchange_and_add( &weak_count_, -1 ) == 1 )
        {
            destroy();
        }
    }

    long use_count() const // nothrow
    {
   //     spinlock_pool<1>::scoped_lock lock( &use_count_ );
        return use_count_;
    }
};

} // namespace detail

} // namespace GZptr

#endif  // #ifndef GZ_SMART_PTR_DETAIL_SP_COUNTED_BASE_SPIN_HPP_INCLUDED
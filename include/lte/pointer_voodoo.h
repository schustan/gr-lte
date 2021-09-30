#ifndef POINTER_VOODOO_H_
#define POINTER_VOODOO_H_

/*
#include <functional>

template<typename T>
void do_release(typename boost::shared_ptr<T> const&, T*)
{
}

template<typename T>
typename std::shared_ptr<T> to_std_ptr(typename boost::shared_ptr<T> const& p)
{
    return
        std::shared_ptr<T>(
                p.get(),
                boost::bind(&do_release<T>, p, std::placeholders::_1));

}*/

/*

template<typename T>
boost::shared_ptr<T> make_shared_ptr(std::shared_ptr<T>& ptr)
{
    return boost::shared_ptr<T>(ptr.get(), [ptr](T*) mutable {ptr.reset();});
}

template<typename T>
std::shared_ptr<T> make_shared_ptr(boost::shared_ptr<T>& ptr)
{
    return std::shared_ptr<T>(ptr.get(), [ptr](T*) mutable {ptr.reset();});
}
*/


// https://github.com/gnuradio/gnuradio/commit/7a9169fe8cca1cb378be0d0d403e03a338ffbfda
// This commit is really troublesome, we just take all pointers and convert them to std::-type within the extension
// https://stackoverflow.com/questions/6326757/conversion-from-boostshared-ptr-to-stdshared-ptr
// Based on Fozi's answer, but only unidirectional
// BE CAREFUL WITH WEAK_PTR
//
namespace {
    template<class SharedPointer> struct Holder {
        SharedPointer p;

        Holder(const SharedPointer &p) : p(p) {}
        Holder(const Holder &other) : p(other.p) {}
        Holder(Holder &&other) : p(std::move(other.p)) {}

        void operator () (...) { p.reset(); }
    };
}

template<class T> std::shared_ptr<T> make_shared_ptr(const boost::shared_ptr<T> &p) {
    typedef Holder<std::shared_ptr<T>> H;
    if(H *h = boost::get_deleter<H>(p)) {
        return h->p;
    } else {
        return std::shared_ptr<T>(p.get(), Holder<boost::shared_ptr<T>>(p));
    }
}

template<class T> std::shared_ptr<T> make_shared_ptr(const std::shared_ptr<T> &p){
    typedef Holder<std::shared_ptr<T>> H;
    if(H * h = std::get_deleter<H>(p)) {
        return h->p;
    } else {
        return std::shared_ptr<T>(p.get(), Holder<std::shared_ptr<T>>(p));
    }
}

#endif // POINTER_VOODOO_H_

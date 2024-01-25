#ifndef swap_h
#define swap_h

/*
 std::swap - меняет местами два параметра одинаковых типа, используя до C++11 оператор копирования копирования, после C++11 оператор перемещения (std::move).
 До С++11, семантика копирования:
 template<class T>
 void swap(T& a, T& b)
 {
     T tmp(a); // две копии объекта a
     a = b;    // две копии объекта b
     b = tmp;  // две копии объекта tmp (т.е. a)
 }
 После С++11, семантика перемещения:
 template<class T>
 void swap(T& a, T& b)
 {
     T tmp = std::move(a);
     a = std::move(b);
     b = std::move(tmp);
 }
 */
namespace SWAP
{
    /// До С++11: семантика копирования
    template<class T>
    void swap_old(T& a, T& b)
    {
        T tmp(a); // две копии объекта a
        a = b;    // две копии объекта b
        b = tmp;  // две копии объекта tmp (т.е. a)
    }
    /// После С++11: семантика перемещения
    template<class T>
    void swap(T& a, T& b)
    {
        T tmp = std::move(a);
        a = std::move(b);
        b = std::move(tmp);
    }
}

#endif /* swap_h */

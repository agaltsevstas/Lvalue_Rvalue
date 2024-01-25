#include "forward.h"
#include "swap.h"

#include <vector>
#include <utility>


/*
 Сайты: https://habr.com/ru/articles/322132/
        https://habr.com/ru/articles/568306/ (комментарии)
 */


int main(int argc, const char * argv[])
{
    /*
     lvalue_rvalue
     lvalue (&) (locator value) - объект, который занимает идентифицируемое место в памяти (адрес) и есть имя (переменная, ссылка &). lvalue может быть стоять как слева, так и справа.
     rvalue (&&) - выражение, которое не представляет собой объект, который занимает идентифицируемое место в памяти и не имеет имени (число, возврат функции). rvalue может стоять только справа.
     Пример:
     int number = 1; // lvalue присваиваем rvalue
     1 = number; // Error, нельзя rvalue присвоить lvalue.
     number = number; // lvalue присваиваем lvalue
     int newNumber = number + number;  // lvalue = rvalue(lvalue + lvalue) - неявное преобразование lvalue->rvalue
     Аргументы функции всегда являются lvalue(xvalue)!!!

     rvalue -  имеет 2 подкатегории: xvalue (eXpiring lvalue) и prvalue (pure rvalue)
     glvalue (generalized lvalue) -  имеет 2 подкатегории: lvalue и xvalue (eXpiring lvalue)
     xvalue - объект, связывающий rvalue с lvalue, но имеет ИМЯ и тип A&&.
     Пример:
     int&& result = function(); // xvalue(A&&) связывает lvalue с rvalue -> lvalue
     template<typename T> function(T&& arg) // xvalue(A&&) связывает lvalue с rvalue -> lvalue

     Приоритет перегрузки:
     1. T&& (rvalue)
     2. const T&& (rvalue, const rvalue) - нигде не используется
     3. T& (lvalue)
     4. const T& (rvalue, const rvalue, lvalue, const lvalue) - константная ссылка продлевает жизнь rvalue-ссылки.

     Отличия:
     - явное пребразование lvalue в rvalue - можно (std::move), rvalue в lvalue - невозможно, исключение неявное преобразование (vector<int>::iterator iter = vec.begin(); *(++iter) = 5;)
     - lvalue можно изменять, а rvalue — нет
     Общее: rvalue ссылка ведет себя точно так же, как и lvalue ссылка, за исключением того, что она может быть связана с временным объектом, тогда как lvalue связать с временным (не константным) объектом нельзя. Например, выход стека: rvalue - объект удалиться, lvalue - может не удалиться.
     
     Скорость:
     template<class T> class vector
     {
        vector (const vector &);              // Конструктор копирования (медленный)
        vector (vector &&);                   // Конструктор переноса из временного объекта (быстрый)
        vector & operator = (const vector &); // Обычное присваивание (медленное)
        vector & operator = (vector &&);      // Перенос временного объекта (быстрый)
        void foo() &;                         // Функция, работающая только для именованного объекта (медленная)
        void foo() &&;                        // Функция, работающая только для временного объекта (быстрая)
     };
     */
    {
        using namespace lvalue_rvalue;
        
        /*
         Приоритет перегрузки функций при lvalue и rvalue
         1. T&& (rvalue)
         2. const T&& (rvalue, const rvalue)
         3. T& (lvalue)
         4. const T& (rvalue, const rvalue, lvalue, const lvalue)
         */
        {
            using namespace priority;
            
            std::cout << "lvalue_rvalue" << std::endl;
            const int&& number1 = function2();
            int number2 = 1;
            const int number3 = 1;
            
            function(function1()); //  1. T&& (rvalue)
            function(function2()); // 1. T&& (rvalue) вместо 2. const T&& (rvalue) TODO: непонятно
            function(number1); // 4. const T& (lvalue) вместо 2. const T&& (rvalue) TODO: непонятно
            function(number2); // 3. T& (lvalue)
            function(number3); // 4. const T& (lvalue)
        }
//        Derived& derivedRef1 = getDerived1(); // Не скомпилируется - нельзя привязать rvalue к ссылке на lvalue
        const Derived& derivedRef2 = getDerived1();
        Derived&& derivedRef3 = getDerived1(); // rvalue == lvalue, потому что xvalue является rvalue, но имеет свойства lvalue
//        Derived&& derivedRef4 = derivedRef3; // Не скомпилируется - нельзя привязать rvalue к ссылке на lvalue
        
        /// Вызывается обычный конструктор без копировнаи и без перемещения, нет смысла вызывать std::move
        Derived derived1 = getDerived1();
        Derived derived2 = getDerived2();

        /// Вызывается конструктор перемещения, нет смысла вызывать std::move для rvalue
        Derived derived3 = getDerived3();

        /// Вызывается конструктор перемещения,  есть смысл вызывать std::move для lvalue
        Derived derived4(std::move(derived3));

        /// Вызывается  автоматически  конструктор перемещения, нет смысла вызывать std::move для rvalue
        derived1 = Derived();
        
        /// Вызывается  конструктор перемещения, есть смысл вызывать std::move для lvalue
        derived1 = std::move(derived2);

        Derived* derived5 = new Derived();
        /// Нет конструктора перемещения у «сырых» указателей, нет смысла вызывать std::move
        Derived* derived6 = derived5;

        std::shared_ptr<Derived> derived7 = std::make_shared<Derived>();
        /// Есть конструктор перемещения у умных указателей, есть смысл вызвать std::move для lvalue
        std::shared_ptr<Derived> derived8 = std::move(derived7);
        
        std::vector<Derived> deriveds;
        /// Вызывается  конструктор перемещения, есть смысл вызывать std::move для lvalue (reserve(1))
        deriveds.emplace_back(std::move(derived1));
        /// 2 Вызова конструктора перемещения, т.к. идет заново выделение памяти (reserve(4))
        deriveds.emplace_back(std::move(derived4));
        /// Вызов обычного конструктора (reserve(4))
        deriveds.emplace_back(derived1);
        /// Вызов обычного конструктора (reserve(4))
        deriveds.emplace_back(2);
        std::cout << "--------------------" << std::endl;
    }
    /*
     std::move преобразует неконстантную lvalue-ссылку или rvalue-ссылку в rvalue-ссылку c помощью конструктора премещения без конструктора копирования. Это просто обертка для static_cast, которая убирает ссылку (& или &&) у переданного аргумента с помощью remove_reference_t и добавляет &&, чтобы преобразовать в тип rvalue.
     template <class _Ty>
     [[nodiscard]] constexpr remove_reference_t<_Ty>&& move(_Ty&& _Arg) noexcept
     {
         return static_cast<remove_reference_t<_Ty>&&>(_Arg);
     }
     Отличие std::move от std::forward: std::move - приводит lvalue к rvalue, std::forward - lvalue просто возвращает lvalue, а rvalue – возвращает std::move(rvalue).
     */
    {
        using namespace MOVE;
        lvalue_rvalue::Derived derived = lvalue_rvalue::getDerived1();
        
        auto newDerived = std::move(derived); // Вызовется конструктор копирования, потому что создается новый объект
        derived = std::move(derived); // Вызывается конструктор перемещения, есть смысл вызывать std::move для lvalue
    }
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
    {
        using namespace SWAP;
        
        std::string str1 = "str1", str2 = "str2";
        int number1 = 1, number2 = 2;
        swap_old(str1, str2); // До С++11: семантика копирования
        swap(str1, str2); // После С++11: семантика перемещения
        swap(number1, number2); // Для int вызовется копирование, т.к.e у int нет конструктора перемещения
    }
    
    /*
     std::forward - идеальной передача (perfect forwarding).
     Свойства:
     - std::forward РАБОТАЕТ с T&& с НЕконстантными ссылками
     - std::forward НЕ работает с const T& - для всех типов ссылок (rvalue, const rvalue, lvalue, const lvalue), т.к. rvalue(T&&) -> lvalue (T&) на основе сжатии ссылок (T&(A&&) = A& // && отбрасывается).
     - std::forward НЕ будет работает c const&& для lvalue

     std::forward используется ТОЛЬКО в шаблонах:
     template<typename T> T&& forward(T&& param)
     {
         if (is_lvalue_reference<T>::value)
             return param;
         else
             return move(param);
     }
     Тип typename T имеет 2 перегрузки:
     1. lvalue-ссылка (T = const A& или T = A&)
     2. rvalue-ссылка (T = A&&)
     3. Бессылочный тип (T = A*) - указатель

     Идеальная передача позволяет создавать функции-обертки, где lvalue-выражения копируются (просто возвращается lvalue), а rvalue-выражения перемещаются (std::move).
     Использование:
     class Example
     {
       std::string field;
       template <class T>
         Example(T&& x) : field(std::forward<T>(x)) {}
     };
     
     reference collapse — сжатие ссылок, которое используется только в шаблонах и определяет поведение (отбрасывает не нужные &) при появлении ссылки на ссылку. Меньшее число (&) - выигрывает у большего числа (&&).
     Пример:
     T инстанцирован A&&, то T&& (A&& &&) = A&& // && отбрасывается
     Правила:
     T&(A&)  = A& // & отбрасывается
     T&&(A&) = A& // && отбрасывается
     T&(A&&) = A& // && отбрасывается
     T&&(A&&) = A&& // && отбрасывается, но тип xvalue->lvalue
     Универсальной ссылка - аргумент типа T&&, где T — шаблонный тип, может принимать оба типа ссылок (A& и A&&).
     Пример:
     template<typename T> function(T&& arg) // arg - lvalue всегда, т.к. T&& arg - xvalue, поэтому будет вызываться конструктор копирования
     Отличие std::forward от std::move: std::move - приводит lvalue к rvalue, std::forward - lvalue просто возвращает lvalue, а rvalue – возвращает std::move(rvalue).
     */
    {
        using namespace FORWARD;
        
        std::cout << "FORWARD" << std::endl;
        int number = 1;
        Function(number); // lvalue: T - int&, arg - int&, std::cout << "&" << "&&" << "&" << std::endl;
        Function(0); // rvalue: T - int, arg - int&&, std::cout << "&" << "&&" << "&&" << std::endl;
        std::cout << "--------------------" << std::endl;
        
        A a;
        std::cout << "--------------------" << std::endl;
        // Без std::forward
        {
            std::cout << "WITHOUT FORWARD" << std::endl;
            auto shared_ptr1 = Make_Shared<A>(a);   // Должно произойти копирование
            auto shared_ptr2 = Make_Shared<A>(A()); // Должно произойти копирование
            std::cout << "--------------------" << std::endl;
        }
        
        // std::forward
        {
            std::cout << "FORWARD" << std::endl;
            auto shared_ptr1 = Make_Shared_Forward<A>(a);   // Должно произойти перемещение
            auto shared_ptr2 = Make_Shared_Forward<A>(A()); // Должно произойти перемещение
            std::cout << "--------------------" << std::endl;
        }
    }
    
    return 0;
}

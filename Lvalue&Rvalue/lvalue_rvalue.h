#ifndef lvalue_rvalue_h
#define lvalue_rvalue_h

#include <iostream>

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

template <typename T>
void log(const T& object, const std::string& msg)
{
    std::cout << "[" << object << "] " << msg << "\n";
}

namespace lvalue_rvalue
{
    namespace priority
    {
        void function(int&& value)
        {
            std::cout << "&&" << std::endl;
        }
        
        void function(const int&& value)
        {
            std::cout << "const &&" << std::endl;
        }
        
        void function(int& value)
        {
            std::cout << "&" << std::endl;
        }

        void function(const int& value)
        {
            std::cout << "const &" << std::endl;
        }

        int function1()
        {
            return 1;
        }

        const int function2()
        {
            return 1;
        }
    }

    class Base
    {
    public:
        explicit Base()
        {
            
        }

        ~Base()
        {
            
        }

        Base(const Base& other)
        {
            
        }
        
        Base& operator=(const Base& other) // Возвращаем ссылку, чтобы потом можно было присвоить
        {
            return *this;
        }
        
        explicit Base(Base&& other) noexcept
        {
            
        }
         
        Base& operator=(Base&& other) // Возвращаем ссылку, чтобы потом можно было присвоить
        {
            return *this;
        }
    };

    class Derived : public Base
    {
    public:
        explicit Derived() : Base()
        {
            log(this, "constructor");
        }
        
        explicit Derived(int) : Base()
        {
            log(this, "constructor");
        }

        ~Derived()
        {
            log(this, "destructor");
        }

        Derived(const Derived& other)
    //        Можно вызвать оператор присваивания, чтобы не писать лишний код
    //        Base(other)
    //        _number(other._number),
    //        _text(other._text)
        {
            log(this, "copy constructor");
            *this = other;
        }
        
        Derived& operator=(const Derived& other) // Возвращаем ссылку, чтобы потом можно было присвоить
        {
            log(this, "copy operator=");
            Base::operator=(other);
            _number = other._number;
            _text = other._text;
            return *this;
        }
        
        explicit Derived(Derived&& other) noexcept
    //    Можно вызвать оператор присваивания, чтобы не писать лишний код
    //    Base(std::move(other))
    //    _number(std::move(other._number)),
    //    _text(std::move(other._text))
        {
            log(this, "move constructor");
            *this = std::move(other);
        }
         
        Derived& operator=(Derived&& other) // Возвращаем ссылку, чтобы потом можно было присвоить
        {
            log(this, "move operator=");
            Base::operator=(std::move(other));
            _number = std::exchange(other._number, 0);
            _text = std::move(other._text);
            return *this;
        }
        
        std::string GetText() const noexcept { return _text; }
        
        int _number = 5;
        std::string _text = "text";
    };

    /// Вызывается обычный конструктор без копировнаи и без перемещения, нет смысла вызывать std::move для rvalue, т.к объект из стека удаляется
    Derived getDerived1()
    {
        Derived derived = Derived();
        return derived;
    }

    /// Вызывается обычный конструктор без копировнаи и без перемещения, нет смысла вызывать std::move для rvalue
    Derived getDerived2()
    {
        return Derived();
    }

    /// Вызовется конструктор копирования вместо operator= перемещения, потому что создается новый объект и он стоит слева(lvalue), нет смысла вызывать std::move для rvalue
    Derived getDerived;
    Derived&& getDerived3()
    {
        return std::move(getDerived);
    }
}

#endif /* lvalue_rvalue_h */

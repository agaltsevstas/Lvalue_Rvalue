#ifndef move_h
#define move_h

/*
 std::move - НИЧЕГО НЕ ПЕРЕМЕЩАЕТ, преобразует неконстантную lvalue-ссылку или rvalue-ссылку в rvalue-ссылку. Это просто обертка для static_cast, которая убирает ссылку (& или &&) у переданного аргумента с помощью remove_reference_t и добавляет &&, чтобы преобразовать в тип rvalue.
 C помощью move можно попасть в конструктор перемещения или оператор присваивания, если он реализован в классе.
 Для const объектов std::move не работает - вызывается конструктор копирования или оператор копирования вместо конструктор перемещения или оператор присваивания.
 template <class _Ty>
 [[nodiscard]] constexpr remove_reference_t<_Ty>&& move(_Ty&& _Arg) noexcept
 {
     return static_cast<remove_reference_t<_Ty>&&>(_Arg);
 }
 Отличие std::move от std::forward: std::move - приводит lvalue к rvalue, std::forward - lvalue просто возвращает lvalue, а rvalue – возвращает std::move(rvalue).
 */
namespace MOVE
{
    template <class T>
    [[nodiscard]] constexpr std::remove_reference_t<T>&& move(T&& value) noexcept
    {
        return static_cast<std::remove_reference_t<T>&&>(value);
    }
}

#endif /* move_h */

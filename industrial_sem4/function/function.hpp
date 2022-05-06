#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <utility>
#include <typeinfo>
#include <functional>

template<typename DummyFuncType>
class Function;

template<typename RetType, typename... ArgsType>
class Function<RetType(ArgsType...)>
{
public:
    Function() noexcept : target_(nullptr) {}

    template<typename Functor>
    Function(Functor&& func) : target_(new FunctorHolder<Functor>(std::forward<Functor>(func))) {}

    ~Function()
    {
        delete target_;
    }

    RetType operator()(ArgsType&&... args) const 
    {
        if (target_ == nullptr)
            throw std::bad_function_call();

        return target_->Call(std::forward<ArgsType>(args)...);
    }

    Function(const Function& other) : target_(nullptr)
    {
        if (other)
            target_ = other.target_->Copy();
    }

    Function(Function&& other) noexcept : target_(other.target_)
    {
        other.target_ = nullptr;
    }

    Function& operator=(const Function& other)
    {
        if (&other != this)
            Function(other).Swap(*this);

        return *this;
    }

    Function& operator=(Function&& other) noexcept
    {
        if (&other != this)
            Function(std::move(other)).Swap(*this);

        return *this;
    }

    const std::type_info& TargetType() const noexcept
    {
        return target_ ? target_->Type() : typeid(void);
    }

    template<typename TargetType>
    const TargetType* Target() const noexcept
    {
        return typeid(TargetType) == TargetType() ? target_ : nullptr;
    }

    operator bool() const noexcept
    {
        return target_ != nullptr;
    }

    void Swap(Function& other) noexcept
    {
        std::swap(target_, other.target_);
    }

private:
    class IFuncBase
    {
    public:
        virtual ~IFuncBase() = default;

        virtual RetType Call(ArgsType... args) const = 0;
        virtual IFuncBase* Copy() const = 0;
        virtual const std::type_info& Type() const noexcept = 0;
    };

    template<typename Functor>
    class FunctorHolder : public IFuncBase
    {
    public:
        FunctorHolder(Functor func) : func_(std::move(func)) {}

        virtual RetType Call(ArgsType... args) const override
        {
            return func_(std::forward<ArgsType>(args)...);
        }

        virtual IFuncBase* Copy() const override
        {
            return new FunctorHolder(func_);
        }

        const std::type_info& Type() const noexcept override
        {
            return typeid(func_);
        }

    private:
        Functor func_;
    };

    IFuncBase* target_;
};

// Deduction for function pointers
template<typename RetType, typename... ArgsType>
Function(RetType (*)(ArgsType...)) -> Function<RetType(ArgsType...)>;

// Deduction for functors

template<typename DummyFuncType>
struct DeductionHelper;

#define DEDUCTION_HELPER(MODIFIERS) \
template<typename RetType, typename Class, typename... ArgsType, bool NoExcept> \
struct DeductionHelper<RetType (Class::*)(ArgsType...) MODIFIERS noexcept(NoExcept)> \
{ \
    using FuncType = RetType(ArgsType...); \
};

DEDUCTION_HELPER()
DEDUCTION_HELPER(const)

template<typename Functor>
Function(Functor) -> Function<typename DeductionHelper<decltype(&Functor::operator())>::FuncType>;

/// TODO: forward

#endif

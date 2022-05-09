#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <utility>
#include <typeinfo>
#include <functional>

template<typename DummyFuncType>
class Function;

template<typename RetType, typename... ArgsType, bool NoExcept>
class Function<RetType(ArgsType...) noexcept(NoExcept)>
{
public:
    Function() noexcept : target_(nullptr) {}

    // For functors
    template<typename Functor>
    Function(Functor func) : target_(new FunctorHolder<Functor>(std::move(func))) {}

    // For class methods
    template<typename MFuncType, typename MClassType>
    Function(MFuncType MClassType::*func) :
        target_(new MethodHolder<MFuncType, MClassType, ArgsType...>(func)) {}

    ~Function()
    {
        delete target_;
    }

    RetType operator()(ArgsType... args) const 
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

    template<typename MFuncType, typename MClassType, typename MClassAccessType, typename... MArgsType>
    class MethodHolder : public IFuncBase
    {
    public:
        using MPtrType = MFuncType MClassType::*;

        static_assert(std::is_same_v<MFuncType, RetType(MArgsType...) noexcept(NoExcept)> ||
                      std::is_same_v<MFuncType, RetType(MArgsType...) const noexcept(NoExcept)>,
                      "Passed class method pointer incompatible with Function instance");

        static_assert(std::is_same_v<MClassAccessType, MClassType&> ||
            std::is_same_v<MClassAccessType, const MClassType&> ||
            std::is_same_v<MClassAccessType, MClassType*> ||
            std::is_same_v<MClassAccessType, const MClassType*>,
            "Class instance type must be (const) reference or pointer (to const)");

        MethodHolder(MPtrType func) : func_(func) {}

        virtual RetType Call(MClassAccessType instance, MArgsType... args) const override
        {
            if constexpr (std::is_pointer_v<MClassAccessType>)
                return (instance->*func_)(std::forward<MArgsType>(args)...);
            else
                return (instance.*func_)(std::forward<MArgsType>(args)...);
        }

        virtual IFuncBase* Copy() const override
        {
            return new MethodHolder(func_);
        }

        const std::type_info& Type() const noexcept override
        {
            return typeid(func_);
        }

    private:
        MPtrType func_;
    };

    IFuncBase* target_;
};

// Deduction guide for function pointers
template<typename RetType, typename... ArgsType, bool NoExcept>
Function(RetType (*)(ArgsType...) noexcept(NoExcept)) ->
    Function<RetType(ArgsType...) noexcept(NoExcept)>;

// Deduction guide for class methods pointers
#define METHOD_DEDUCTION_HELPER(MODIFIERS) \
template<typename MRetType, typename MClassType, typename... MArgsType, bool NoExcept> \
Function(MRetType (MClassType::*)(MArgsType...) MODIFIERS noexcept(NoExcept)) -> \
    Function<MRetType(MClassType&, MArgsType...) noexcept(NoExcept)>;

METHOD_DEDUCTION_HELPER()
METHOD_DEDUCTION_HELPER(const)

// Deduction guide for functors

template<typename DummyFuncType>
struct DeductionHelper;

#define FUNCTOR_DEDUCTION_HELPER(MODIFIERS) \
template<typename RetType, typename Class, typename... ArgsType, bool NoExcept> \
struct DeductionHelper<RetType (Class::*)(ArgsType...) MODIFIERS noexcept(NoExcept)> \
{ \
    using FuncType = RetType(ArgsType...) noexcept(NoExcept); \
};

FUNCTOR_DEDUCTION_HELPER()
FUNCTOR_DEDUCTION_HELPER(const)

template<typename Functor>
Function(Functor) -> Function<typename DeductionHelper<decltype(&Functor::operator())>::FuncType>;

#endif

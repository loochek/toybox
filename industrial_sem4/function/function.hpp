#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <cassert>
#include <utility>
#include <typeinfo>
#include <functional>

template<typename DummyFuncType>
class Function;

template<typename RetType, typename... ArgsType, bool NoExcept>
class Function<RetType(ArgsType...) noexcept(NoExcept)>
{
    static constexpr size_t SFO_MAX_SIZE = 64;

    class IFuncBase;

public:
    Function() noexcept : target_(nullptr), state_(State::Empty) {}

    // For functors
    template<typename Functor>
    Function(Functor func) : state_(State::Empty)
    {
        if constexpr (sizeof(FunctorHolder<Functor>) <= SFO_MAX_SIZE)
        {
            new (sfo_buf_) FunctorHolder<Functor>(std::move(func));
            state_ = State::Small;
        }
        else
        {
            target_ = new FunctorHolder<Functor>(std::move(func));
            state_ = State::Dynamic;
        }
    }            

    // For class methods
    template<typename MFuncType, typename MClassType>
    Function(MFuncType MClassType::*func) : state_(State::Small)
    {
        assert(sizeof(MethodHolder<MFuncType, MClassType, ArgsType...>) <= SFO_MAX_SIZE);
        new (sfo_buf_) MethodHolder<MFuncType, MClassType, ArgsType...>(func);
    }

    ~Function()
    {
        switch (state_)
        {
        case State::Small:
            reinterpret_cast<IFuncBase*>(sfo_buf_)->~IFuncBase();
            break;

        case State::Dynamic:
            delete target_;
            break;

        default:
            break;
        }

        target_ = nullptr;
        state_ = State::Empty;
    }

    RetType operator()(ArgsType... args) const 
    {
        if (state_ == State::Empty)
            throw std::bad_function_call();

        return GetTarget()->Call(std::forward<ArgsType>(args)...);
    }

    Function(const Function& other) : target_(nullptr), state_(State::Empty)
    {
        other.GetTarget()->Copy(*this);
        // state_ is handled by Copy()
    }

    Function(Function&& other) noexcept : target_(nullptr), state_(State::Empty)
    {
        other.GetTarget()->Move(*this, other);
        // state_ is handled by Move()
        // other state is handled by Move()
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
        return state_ == State::Empty ? typeid(void) : GetTarget()->Type();
    }

    template<typename TargetType>
    const TargetType* Target() const noexcept
    {
        return typeid(TargetType) == TargetType() ? GetTarget() : nullptr;
    }

    operator bool() const noexcept
    {
        return state_ != State::Empty;
    }

    void Swap(Function& other) noexcept
    {
        std::swap(target_, other.target_);
        std::swap(state_, other.state_);
    }

private:
    IFuncBase* GetTarget() noexcept
    {
        return state_ == State::Small ? reinterpret_cast<IFuncBase*>(sfo_buf_) : target_;
    }

    const IFuncBase* GetTarget() const noexcept
    {
        return state_ == State::Small ? reinterpret_cast<const IFuncBase*>(sfo_buf_) : target_;
    }

    class IFuncBase
    {
    public:
        virtual ~IFuncBase() = default;

        virtual RetType Call(ArgsType... args) const = 0;
        virtual void Copy(Function& to) const = 0;
        virtual void Move(Function& to, Function& from) = 0;
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

        virtual void Copy(Function& to) const override
        {
            if constexpr (sizeof(FunctorHolder) <= SFO_MAX_SIZE)
            {
                new (to.sfo_buf_) FunctorHolder(func_);
                to.state_ = State::Small;
            }
            else
            {
                to.target_ = new FunctorHolder(func_);
                to.state_ = State::Dynamic;
            }
        }

        virtual void Move(Function& to, Function& from) override
        {
            if constexpr (sizeof(FunctorHolder) <= SFO_MAX_SIZE)
            {
                new (to.sfo_buf_) FunctorHolder(std::move(func_));
                to.state_ = State::Small;

                if (from.state_ == State::Small)
                    this->~FunctorHolder();
                else
                    delete from.target_;
            }
            else
            {
                if (from.state_ == State::Dynamic)
                    to.target_ = from.target_;
                else
                {
                    new (to.sfo_buf_) FunctorHolder(std::move(func_));
                    delete from.target_;
                }

                to.state_ = State::Dynamic;
            }

            from.target_ = nullptr;
            from.state_ = State::Empty;
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
        static_assert(std::is_same_v<MFuncType, RetType(MArgsType...) noexcept(NoExcept)> ||
                std::is_same_v<MFuncType, RetType(MArgsType...) const noexcept(NoExcept)>,
                "Passed class method pointer incompatible with Function instance");

        static_assert(std::is_same_v<MClassAccessType, MClassType&> ||
            std::is_same_v<MClassAccessType, const MClassType&> ||
            std::is_same_v<MClassAccessType, MClassType*> ||
            std::is_same_v<MClassAccessType, const MClassType*>,
            "Class instance type must be (const) reference or pointer (to const)");

        using MPtrType = MFuncType MClassType::*;

    public:

        MethodHolder(MPtrType func) : func_(func)
        {
            assert(sizeof(MethodHolder) <= SFO_MAX_SIZE);
        }

        virtual RetType Call(MClassAccessType instance, MArgsType... args) const override
        {
            if constexpr (std::is_pointer_v<MClassAccessType>)
                return (instance->*func_)(std::forward<MArgsType>(args)...);
            else
                return (instance.*func_)(std::forward<MArgsType>(args)...);
        }

        virtual void Copy(Function& to) const override
        {
            new (to.sfo_buf_) MethodHolder(func_);
            to.state_ = State::Small;
        }

        virtual void Move(Function& to, Function& from) override
        {
            assert(from.state_ == State::Small);

            new (to.sfo_buf_) MethodHolder(func_);
            to.state_ = State::Small;

            this->~MethodHolder();

            from.target_ = nullptr;
            from.state_ = State::Empty;
        }

        const std::type_info& Type() const noexcept override
        {
            return typeid(func_);
        }

    private:
        MPtrType func_;
    };

public:
    union
    {
        IFuncBase* target_;
        uint8_t sfo_buf_[SFO_MAX_SIZE];
    };

    enum class State
    {
        Empty,
        Small,
        Dynamic
    };

    State state_;
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

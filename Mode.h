#ifndef INC_LUTIL_MODE_H_
#define INC_LUTIL_MODE_H_
/**
@file Mode.h
@author t-sakai
@date 2016/11/10 create
*/
#include <cassert>
#include <cstdint>

namespace lutil
{
typedef int32_t s32;

#define LUTIL_MODE_DECL(CLASS_NAME, STATE_NUM, TYPENAME) \
    friend class lutil::Mode<CLASS_NAME, STATE_NUM>; \
    template<lutil::s32 N> \
    void init(); \
    template<lutil::s32 N> \
    void proc(); \
    template<lutil::s32 N> \
    void term(); \
    lutil::Mode<CLASS_NAME, STATE_NUM> TYPENAME

#define LUTIL_MODE_DESCLSTATEFUNC(STATE_NAME) \
    template<> \
    void init<STATE_NAME>(); \
    template<> \
    void proc<STATE_NAME>(); \
    template<> \
    void term<STATE_NAME>()

#define LUTIL_MODE_IMPL_INIT(CLASS_NAME, STATE_NAME) template<> \
void CLASS_NAME::init<CLASS_NAME::STATE_NAME>()

#define LUTIL_MODE_IMPL_PROC(CLASS_NAME, STATE_NAME) template<> \
void CLASS_NAME::proc<CLASS_NAME::STATE_NAME>()

#define LUTIL_MODE_IMPL_TERM(CLASS_NAME, STATE_NAME) template<> \
void CLASS_NAME::term<CLASS_NAME::STATE_NAME>()

template<class T, s32 N>
class Mode
{
public:
    static_assert(0 < N, "N should be bigger than zero");
    typedef Mode<T, N> this_type;

    explicit Mode(T* parent);

    Mode(T* parent, s32 state);

    ~Mode();

    s32 get() const;

    void set(s32 state);

    void initialize(s32 state);

    void update();

private:
    template<class T, class U, int N>
    struct FuncInitializer: FuncInitializer<T, U, N - 1>
    {
        FuncInitializer()
        {
            T::inits_[N] = &U::init<N>;
            T::procs_[N] = &U::proc<N>;
            T::terms_[N] = &U::term<N>;
        }
    };

    template<class T, class U>
    struct FuncInitializer<T, U, 0>
    {
        FuncInitializer()
        {
            T::inits_[0] = &U::init<0>;
            T::procs_[0] = &U::proc<0>;
            T::terms_[0] = &U::term<0>;
        }
    };

    typedef void (T::*func_type)();
    typedef FuncInitializer<this_type, T, N - 1> FuncInitializerType;

    s32 state_;
    T* parent_;

    static FuncInitializerType funcInitializer_;
    static func_type inits_[N];
    static func_type procs_[N];
    static func_type terms_[N];
};

template<class T, s32 N>
typename Mode<T, N>::FuncInitializerType Mode<T, N>::funcInitializer_;

template<class T, s32 N>
typename Mode<T, N>::func_type Mode<T, N>::inits_[N];

template<class T, s32 N>
typename Mode<T, N>::func_type Mode<T, N>::procs_[N];

template<class T, s32 N>
typename Mode<T, N>::func_type Mode<T, N>::terms_[N];

template<class T, s32 N>
Mode<T, N>::Mode(T* parent)
    : state_(-1)
    , parent_(parent)
{
    assert(nullptr != parent_);
    FuncInitializerType funcInitializer;
}

template<class T, s32 N>
Mode<T, N>::Mode(T* parent, s32 state)
    : state_(state)
    , parent_(parent)
{
    assert(0 <= state && state < N);
    assert(nullptr != parent_);
    FuncInitializerType funcInitializer;
    (parent_->*inits_[state_])();
}

template<class T, s32 N>
Mode<T, N>::~Mode()
{
    (parent_->*terms_[state_])();
}

template<class T, s32 N>
void Mode<T, N>::initialize(s32 state)
{
    assert(0 <= state && state < N);
    state_ = state;
    (parent_->*inits_[state_])();
}

template<class T, s32 N>
void Mode<T, N>::update()
{
    assert(0 <= state_ && state_ < N);
    (parent_->*procs_[state_])();
}

template<class T, s32 N>
s32 Mode<T, N>::get() const
{
    return state_;
}

template<class T, s32 N>
void Mode<T, N>::set(s32 state)
{
    assert(0 <= state && state < N);

    (parent_->*terms_[state_])();
    state_ = state;
    for(;;) {
        s32 prev = state_;
        (parent_->*inits_[prev])();
        if(state_ == prev) {
            break;
        }
    }
}

template<class T, s32 N>
class ModeProc
{
public:
    static_assert(0 < N, "N should be bigger than zero");
    typedef ModeProc<T, N> this_type;

    ModeProc(T* parent, s32 state = 0);

    ~ModeProc();

    s32 get() const;

    void set(s32 state);

    void update();

private:
    template<class T, class U, int N>
    struct FuncInitializer: FuncInitializer<T, U, N - 1>
    {
        FuncInitializer()
        {
            T::procs_[N] = &U::proc<N>;
        }
    };

    template<class T, class U>
    struct FuncInitializer<T, U, 0>
    {
        FuncInitializer()
        {
            T::procs_[0] = &U::proc<0>;
        }
    };

    typedef void (T::*func_type)();
    typedef FuncInitializer<this_type, T, N - 1> FuncInitializerType;

    s32 state_;
    T* parent_;

    static func_type procs_[N];
};

template<class T, s32 N>
typename ModeProc<T, N>::func_type ModeProc<T, N>::procs_[N];

template<class T, s32 N>
ModeProc<T, N>::ModeProc(T* parent, s32 state)
    : state_(state)
    , parent_(parent)
{
    assert(0 <= state_ && state_ < N);
    assert(nullptr != parent_);
    FuncInitializerType funcInitializer;
}

template<class T, s32 N>
ModeProc<T, N>::~ModeProc() {}

template<class T, s32 N>
void ModeProc<T, N>::update()
{
    assert(0 <= state_ && state_ < N);
    (parent_->*procs_[state_])();
}

template<class T, s32 N>
s32 ModeProc<T, N>::get() const
{
    return state_;
}

template<class T, s32 N>
void ModeProc<T, N>::set(s32 state)
{
    assert(0 <= state && state < N);
    state_ = state;
}
} // namespace lutil
#endif // INC_LUTIL_MODE_H_

#include <iostream>
#include "../Mode.h"

class Test2
{
public:
    enum State
    {
        State_0 =0,
        State_1,
        State_Num,
    };

    Test2()
        :mode_(this, 0)
    {}

    void update()
    {
        mode_.update();
    }
private:
    LUTIL_MODE_DECL(Test2, State_Num, mode_);

    LUTIL_MODE_DESCLSTATEFUNC(State_0);
    LUTIL_MODE_DESCLSTATEFUNC(State_1);
};

LUTIL_MODE_IMPL_INIT(Test2, State_0)
{
    std::cout << "init<0>" << std::endl;
}

LUTIL_MODE_IMPL_PROC(Test2, State_0)
{
    std::cout << "proc<0>" << std::endl;
    mode_.set(State_1);
}

LUTIL_MODE_IMPL_TERM(Test2, State_0)
{
    std::cout << "term<0>" << std::endl;
}

LUTIL_MODE_IMPL_INIT(Test2, State_1)
{
    std::cout << "init<1>" << std::endl;
}

LUTIL_MODE_IMPL_PROC(Test2, State_1)
{
    std::cout << "proc<1>" << std::endl;
}

LUTIL_MODE_IMPL_TERM(Test2, State_1)
{
    std::cout << "term<1>" << std::endl;
}

int main(void)
{
    Test2 t2;
    t2.update();
    return 0;
}

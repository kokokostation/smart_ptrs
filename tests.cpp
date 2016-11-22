#include <gtest/gtest.h>
#include <vector>

#include "tests.h"
#include "smart_ptr.h"
#include "exception.h"

using namespace tuz;

class Testing_class
{
private:
    int* counter;
    int var;
    Testing_class(const Testing_class&) = delete;
    void operator=(const Testing_class&) = delete;

public:
    int public_field;

    int get_var() const
    {
        return var;
    };
    explicit Testing_class(int* counter, int var = 228) : counter(counter), var(var), public_field(0) {};
    ~Testing_class()
    {
        ++*counter;
    };
};

class Esft_test : public enable_shared_from_this<Esft_test>
{
public:
    int var;

    explicit Esft_test(int var) : var(var) {};
};

template<typename T>
class Test_deleter
{
    int mark;
    int* counter;

public:
    explicit Test_deleter(int mark, int* counter = nullptr) : mark(mark), counter(counter) {};
    int get_mark() const
    {
        return mark;
    };
    void operator()(T* ptr)
    {
        if(counter)
            ++*counter;
        delete ptr;
    };
};

//custom deleter, unique_ptr()
TEST(unique_ptr, test_1)
{
    unique_ptr<int> plain_up;

    unique_ptr<int, Test_deleter<int>> up(new int, Test_deleter<int>(15)), up1(new int, Test_deleter<int>(77));
    std::swap(up, up1);

    EXPECT_EQ(up.get_deleter().get_mark(), 77);
    EXPECT_EQ(up1.get_deleter().get_mark(), 15);
}

//vector<unique_ptr> move
TEST(unique_ptr, test_2)
{
    int counter = 0;

    {
        std::vector<unique_ptr<Testing_class>> a, b;
        for(size_t i = 0; i < 10; ++i)
            a.push_back(unique_ptr<Testing_class>(new Testing_class(&counter, i)));

        b = std::move(a);
        EXPECT_EQ(a.empty(), true);
        EXPECT_EQ(counter, 0);
    }

    EXPECT_EQ(counter, 10);
}

//make_unique, operator*, operator->
TEST(unique_ptr, test_3)
{
    int counter = 0;

    {
        unique_ptr<Testing_class> up = make_unique<Testing_class>(&counter, 2);

        EXPECT_EQ(up->get_var(), 2);
        EXPECT_EQ((*up).public_field, up->public_field);
    }

    EXPECT_EQ(counter, 1);
}

//make_unique, unique_ptr(unique_ptr&&), std::swap(unique_ptr&, unique_ptr&), get
TEST(unique_ptr, test_4)
{
    int counter = 0;

    {
        unique_ptr<Testing_class>  up = make_unique<Testing_class>(&counter, 4),
                                   up1(std::move(up));
        std::swap(up, up1);

        EXPECT_EQ(up->get_var(), 4);
        EXPECT_EQ(up1.get(), nullptr);
    }

    EXPECT_EQ(counter, 1);
}

//reset
TEST(unique_ptr, test_5)
{
    int counter = 0;

    {
        unique_ptr<Testing_class> up;
        up.reset(new Testing_class(&counter, 6));
        EXPECT_EQ(counter, 0);

        up.reset(new Testing_class(&counter, 8));
        EXPECT_EQ(counter, 1);

        up.reset();
        EXPECT_EQ(counter, 2);
    }

    EXPECT_EQ(counter, 2);
}

//get_deleter
TEST(unique_ptr, test_6)
{
    int counter = 0;

    {
        unique_ptr<Testing_class> up;
        up.get_deleter()(new Testing_class(&counter));

        EXPECT_EQ(counter, 1);
    }
}

//bool(), release, get, comparing operators, operator=(unique_ptr&&)
TEST(unique_ptr, test_7)
{
    int counter = 0;

    {
        unique_ptr<Testing_class> up = make_unique<Testing_class>(&counter, 4), up1;
        if(up)
            delete up.release();

        EXPECT_EQ(counter, 1);
        EXPECT_EQ(up.get(), nullptr);

        if(!up)
            up = tuz::make_unique<Testing_class>(&counter, 10);

        EXPECT_EQ(up->get_var(), 10);

        EXPECT_TRUE(up >= up1);
        EXPECT_FALSE(up <= up1);
        EXPECT_FALSE(up < up1);
        EXPECT_TRUE(up > up1);
        EXPECT_TRUE(up != up1);

        up.reset();

        EXPECT_TRUE(up == up1);
        EXPECT_EQ(counter, 2);
    }
}

//shared_ptr()
TEST(shared_ptr, test_1)
{
    int* int_ptr = nullptr;
    unique_ptr<int> up(nullptr);
    shared_ptr<int>    a,
               b(nullptr),
               c(int_ptr),
               d(std::move(up)),
               e(NULL);

    EXPECT_EQ(a.DEBUG_get_proxy_addr(), Proxy_dummy<int>::instance_ptr());
    EXPECT_EQ(b.DEBUG_get_proxy_addr(), Proxy_dummy<int>::instance_ptr());
    EXPECT_EQ(c.DEBUG_get_proxy_addr(), Proxy_dummy<int>::instance_ptr());
    EXPECT_EQ(d.DEBUG_get_proxy_addr(), Proxy_dummy<int>::instance_ptr());
    EXPECT_EQ(e.DEBUG_get_proxy_addr(), Proxy_dummy<int>::instance_ptr());
}

//make_shared, shared_ptr.operator=(shared_ptr&&), shared_ptr.use_count
TEST(shared_ptr, test_2)
{
    int counter = 0;

    {
        shared_ptr<Testing_class> sp = make_shared<Testing_class>(&counter, 2), sp1;

        EXPECT_EQ(sp->get_var(), 2);
        EXPECT_EQ(sp.use_count(), 1);

        sp1 = std::move(sp);

        EXPECT_EQ(sp1.use_count(), 1);
        EXPECT_FALSE(sp);
        EXPECT_EQ(sp1->get_var(), 2);
    }

    EXPECT_EQ(counter, 1);
}

//shared_ptr.operator=(unique_ptr&&), shared_ptr.get(), shared_ptr.operator*(), shared_ptr.operator->()
TEST(shared_ptr, test_3)
{
    int counter = 0;

    {
        unique_ptr<Testing_class> up(new Testing_class(&counter, 3));
        shared_ptr<Testing_class> sp;
        sp = std::move(up);

        EXPECT_EQ(up.get(), nullptr);
        EXPECT_EQ(sp.use_count(), 1);
        EXPECT_EQ(sp.get()->get_var(), 3);

        sp.get()->public_field = 7;

        EXPECT_EQ(sp->public_field, 7);
        EXPECT_EQ((*sp).public_field, 7);
    }

    EXPECT_EQ(counter, 1);
}

//std::swap(shared_ptr&, shared_ptr&), shared_ptr.operator=(const shared_ptr&)
TEST(shared_ptr, test_4)
{
    int counter = 0;

    {
        shared_ptr<Testing_class>   sp = make_shared<Testing_class>(&counter, 3),
                                    sp1 = make_shared<Testing_class>(&counter, 2);

        std::swap(sp, sp1);

        EXPECT_EQ(sp1->get_var(), 3);

        sp = sp1;

        EXPECT_EQ(counter, 1);
    }

    EXPECT_EQ(counter, 2);
}

//shared_ptr.reset(), shared_ptr.operator bool(), comparing operators
TEST(shared_ptr, test_5)
{
    int counter = 0;

    {
        shared_ptr<Testing_class>   sp = make_shared<Testing_class>(&counter, 3),
                                    sp1 = make_shared<Testing_class>(&counter, 2);

        if(sp)
            sp.reset();

        EXPECT_EQ(counter, 1);

        EXPECT_FALSE(sp >= sp1);
        EXPECT_TRUE(sp <= sp1);
        EXPECT_TRUE(sp < sp1);
        EXPECT_FALSE(sp > sp1);
        EXPECT_TRUE(sp != sp1);
        EXPECT_FALSE(sp == sp1);

        shared_ptr<Testing_class> sp2(sp1);

        EXPECT_EQ(sp2.use_count(), 2);

        if(sp1 && !sp)
            sp1.reset();

        EXPECT_EQ(sp2.use_count(), 1);
        EXPECT_EQ(counter, 1);
        EXPECT_FALSE(sp1 || sp);
    }
}

//shared_ptr(shared_ptr&&), shared_ptr.reset(T*, D)
TEST(shared_ptr, test_6)
{
    int counter = 0;

    {
        shared_ptr<Testing_class>  sp = make_shared<Testing_class>(&counter, 4),
                                   sp2(std::move(sp));

        EXPECT_EQ(sp2.use_count(), 1);
        EXPECT_EQ(sp.get(), nullptr);

        sp2.reset(new Testing_class(&counter));

        EXPECT_EQ(counter, 1);
        EXPECT_EQ(sp2->get_var(), 228);

        sp2.reset();

        EXPECT_EQ(counter, 2);
    }
}

//weak_ptr.lock(), weak_ptr.use_count(), weak_ptr(const shared_ptr&), shared_ptr(const shared_ptr&), weak_ptr(const weak_ptr&)
TEST(shared_ptr_and_weak_ptr, test_1)
{
    int counter = 0;

    {
        shared_ptr<Testing_class> sp = make_shared<Testing_class>(&counter, 2);
        weak_ptr<Testing_class> wp(sp), wp1(wp);
        shared_ptr<Testing_class> sp1(wp.lock());

        EXPECT_EQ(sp.use_count(), 2);
        EXPECT_EQ(wp.use_count(), 2);
        EXPECT_EQ(sp1->get_var(), 2);
        EXPECT_EQ(wp.DEBUG_weak_links_count(), 2);
    }

    EXPECT_EQ(counter, 1);
}

//weak_ptr.operator=(const shared_ptr&), weak_ptr.operator=(const weak_ptr&), weak_ptr.reset(), std::swap(weak_ptr&, weak_ptr&)
TEST(shared_ptr_and_weak_ptr, test_2)
{
    int counter = 0;

    weak_ptr<Testing_class> wp;

    {
        shared_ptr<Testing_class> sp = make_shared<Testing_class>(&counter, 2), sp1 = make_shared<Testing_class>(&counter, 3);
        weak_ptr<Testing_class> wp1, wp2, wp3(sp1);

        wp = sp;
        wp1 = wp;
        wp2 = sp;

        EXPECT_EQ(wp.DEBUG_weak_links_count(), 3);
        EXPECT_EQ(sp.use_count(), 1);

        wp1.reset();

        EXPECT_EQ(wp.DEBUG_weak_links_count(), 2);
        EXPECT_EQ(wp2.DEBUG_weak_links_count(), 2);

        std::swap(wp3, wp2);

        EXPECT_EQ(wp3.lock()->get_var(), 2);
        EXPECT_EQ(wp2.lock()->get_var(), 3);
    }

    EXPECT_EQ(wp.use_count(), 0);
    EXPECT_EQ(wp.DEBUG_weak_links_count(), 1);
    EXPECT_EQ(counter, 2);
}

//shared_ptr(const weak_ptr&)
TEST(shared_ptr_and_weak_ptr, test_3)
{
    int counter = 0;

    {
        shared_ptr<Testing_class> sp = make_shared<Testing_class>(&counter, 4);
        weak_ptr<Testing_class> wp(sp);

        EXPECT_EQ(wp.use_count(), 1);
        EXPECT_EQ(wp.DEBUG_weak_links_count(), 1);

        EXPECT_NO_THROW(shared_ptr<Testing_class> sp(wp));

        sp.reset();

        EXPECT_THROW(shared_ptr<Testing_class> sp(wp), bad_weak_ptr);

        EXPECT_EQ(counter, 1);
    }
}

//проверка на то, что не крашится с тяжелым deleter
TEST(shared_ptr_and_weak_ptr, test_4)
{
    int d_counter = 0, t_counter = 0;

    {
        unique_ptr<Testing_class, Test_deleter<Testing_class>> up(new Testing_class(&t_counter, 3), Test_deleter<Testing_class>(5, &d_counter));
        shared_ptr<Testing_class> sp(std::move(up));

        EXPECT_EQ(sp->get_var(), 3);
        EXPECT_EQ(t_counter, 0);
        EXPECT_EQ(d_counter, 0);

        weak_ptr<Testing_class> wp(sp);
        sp.reset();

        EXPECT_EQ(t_counter, 1);
        EXPECT_EQ(d_counter, 1);
    }
}

//self-assignment problem
TEST(shared_ptr_and_weak_ptr, test_5)
{
    shared_ptr<int> sp = make_shared<int>(9);

    EXPECT_NO_THROW(sp = sp);

    weak_ptr<int> wp(sp);

    sp.reset();

    EXPECT_NO_THROW(wp = wp);
}

//Вспомогательная функция.
void esft_test_helper(shared_ptr<Esft_test>& sp, Esft_test* esft)
{
    EXPECT_EQ(sp->var, esft->var);

    weak_ptr<Esft_test> wp = sp;
    shared_ptr<Esft_test> sp1(sp);

    EXPECT_EQ(sp.use_count(), 2);
    EXPECT_EQ(wp.DEBUG_weak_links_count(), 2); //там ещё в esft weak_ptr лежит

    shared_ptr<Esft_test> sp2 = esft->shared_from_this();

    EXPECT_EQ(wp.use_count(), 3);
    EXPECT_EQ(wp.DEBUG_weak_links_count(), 2);
    EXPECT_EQ(sp->var, sp2->var);
}

//проверка для простого конструктора
TEST(enable_shared_from_this, test_1)
{
    weak_ptr<Esft_test> wp;

    {
        Esft_test* esft = new Esft_test(5);
        shared_ptr<Esft_test> sp(esft);

        esft_test_helper(sp, sp.get());

        wp = sp;
        EXPECT_EQ(wp.DEBUG_weak_links_count(), 2);
    }

    EXPECT_EQ(wp.DEBUG_weak_links_count(), 1); //объект удалился, значит и weak_ptr в нем тоже
}

//проверка для make_shared
TEST(enable_shared_from_this, test_2)
{
    shared_ptr<Esft_test> sp = make_shared<Esft_test>(5);
    esft_test_helper(sp, sp.get());
}

int test(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

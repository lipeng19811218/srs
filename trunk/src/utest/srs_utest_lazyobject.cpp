

#include <srs_utest_lazyobject.hpp>
#include <srs_core.hpp>
#include <srs_app_conn.hpp>

class Share1 : public SrsLazyObject, ISrsResource 
{
public:
    bool valid_;    
    uint8_t* p_;
public:
    Share1(SrsLazyObjectWrapper<Share1>* root):valid_(true), p_(new uint8_t[8])
    {}
    virtual ~Share1(){ delete[] p_;}
    virtual const SrsContextId& get_id() { return _srs_context->get_id();}
    virtual std::string desc() { return "share1";}
};

VOID TEST(LazyObject, sharePtr) 
{
    SrsLazyObjectWrapper<Share1>* s1 = new SrsLazyObjectWrapper<Share1>;
    SrsLazyObjectWrapper<Share1>* s2 = s1->copy();

    delete s1;
    EXPECT_TRUE(s2->resource()->valid_);
    EXPECT_TRUE(s2->resource()->p_ != NULL);
    delete s2;
}

VOID TEST(LazyObject, weakPtr) 
{
    SrsLazyObjectWrapper<Share1>* s1 = new SrsLazyObjectWrapper<Share1>;
    SrsWeakLazyObjectWrapper<Share1>* w1 = new SrsWeakLazyObjectWrapper<Share1>(s1->resource());
    EXPECT_TRUE(w1->is_valid());
    EXPECT_TRUE(w1->resource()->p_ != NULL);
    delete s1;

    EXPECT_FALSE(w1->is_valid());
    delete w1;
}

VOID TEST(LazyObject, multiWeakPtr) 
{
    SrsLazyObjectWrapper<Share1>* s1 = new SrsLazyObjectWrapper<Share1>;
    SrsWeakLazyObjectWrapper<Share1>* w1 = new SrsWeakLazyObjectWrapper<Share1>(s1->resource());
    SrsWeakLazyObjectWrapper<Share1>* w2 = new SrsWeakLazyObjectWrapper<Share1>(s1->resource());
    SrsWeakLazyObjectWrapper<Share1>* w3 = new SrsWeakLazyObjectWrapper<Share1>(s1->resource());
    EXPECT_TRUE(w1->is_valid());
    EXPECT_TRUE(w1->resource()->p_ != NULL);
    EXPECT_TRUE(w2->is_valid());
    EXPECT_TRUE(w2->resource()->p_ != NULL);
    EXPECT_TRUE(w3->is_valid());
    EXPECT_TRUE(w3->resource()->p_ != NULL);
    delete s1;

    EXPECT_FALSE(w1->is_valid());
    EXPECT_FALSE(w2->is_valid());
    EXPECT_FALSE(w3->is_valid());
    delete w1;
    delete w2;
    delete w3;
}

VOID TEST(LazyObject, autoWeakPtr)
{
    SrsLazyObjectWrapper<Share1>* s1 = new SrsLazyObjectWrapper<Share1>;
    SrsWeakLazyObjectWrapper<Share1>* w1 = new SrsWeakLazyObjectWrapper<Share1>(s1->resource());
    EXPECT_TRUE(w1->is_valid());
    EXPECT_TRUE(w1->resource()->p_ != NULL);
    if(true) {
        SrsAutoLockWeakLazyObject<Share1> lock(w1);
        delete s1;
        EXPECT_TRUE(w1->is_valid());
        EXPECT_TRUE(w1->resource()->p_ != NULL);
    }
    EXPECT_FALSE(w1->is_valid());
    delete w1;
}

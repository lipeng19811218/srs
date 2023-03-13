//
// Copyright (c) 2013-2023 The SRS Authors
//
// SPDX-License-Identifier: MIT or MulanPSL-2.0
//

#include <srs_utest_shared_ptr.hpp>
#include <srs_app_shared_ptr.hpp>
#include <srs_core_autofree.hpp>


class TestShared
{
public:
    bool valid_;
    uint8_t* p_;
public:
    TestShared(): valid_(true), p_(new uint8_t[8])
    {}
    virtual ~TestShared(){
        valid_ = false;
        delete []p_;
    }
};


VOID TEST(SharedPTR, share_ptr) {
    SrsSharedPtr<TestShared>* s1 = new SrsSharedPtr<TestShared>(new TestShared());
    SrsSharedPtr<TestShared>* s2 = s1->copy();

    delete s1;
    EXPECT_TRUE(s2->get()->valid_);
    EXPECT_TRUE(s2->get()->p_ != NULL);
    delete s2;
}

VOID TEST(SharedPTR, weak_ptr) {
    SrsSharedPtr<TestShared>* s1 = new SrsSharedPtr<TestShared>(new TestShared());
    SrsWeakPtr<TestShared>* w1 = new SrsWeakPtr<TestShared>(*s1);
    if(true) {
        SrsSharedPtr<TestShared>* s2 = w1->lock();
        if(s2 != NULL) {
            SrsAutoFree(SrsSharedPtr<TestShared>, s2);
            EXPECT_TRUE(s2->get()->valid_);
            EXPECT_TRUE(s2->get()->p_ != NULL);
        }
    }
    delete s1;
    EXPECT_TRUE(NULL == w1->lock());
    srs_freep(w1);
}


VOID TEST(SharedPTR, multiWeakPtr)
{
    SrsSharedPtr<TestShared>* s1 = new SrsSharedPtr<TestShared>(new TestShared());
    SrsWeakPtr<TestShared>* w1 = new SrsWeakPtr<TestShared>(*s1);
    SrsWeakPtr<TestShared>* w2 = new SrsWeakPtr<TestShared>(*s1);
    SrsWeakPtr<TestShared>* w3 = w1->copy();
    EXPECT_FALSE(w1->expire());
    EXPECT_FALSE(w2->expire());
    EXPECT_FALSE(w3->expire());
    if(true) {
        SrsSharedPtr<TestShared>* lock = w1->lock();
        SrsAutoFree(SrsSharedPtr<TestShared>, lock);
        EXPECT_TRUE(lock->get()->p_ != NULL);
    }
    if(true) {
        SrsSharedPtr<TestShared>* lock = w2->lock();
        SrsAutoFree(SrsSharedPtr<TestShared>, lock);
        EXPECT_TRUE(lock->get()->p_ != NULL);
    }
    if(true) {
        SrsSharedPtr<TestShared>* lock = w3->lock();
        SrsAutoFree(SrsSharedPtr<TestShared>, lock);
        EXPECT_TRUE(lock->get()->p_ != NULL);
    }

    srs_freep(s1);

    EXPECT_TRUE(w1->expire());
    EXPECT_TRUE(w2->expire());
    EXPECT_TRUE(w3->expire());

    srs_freep(w1);
    srs_freep(w2);
    srs_freep(w3);

}

VOID TEST(SharedPTR, lockWeakPTR)
{
    SrsSharedPtr<TestShared>* s1 = new SrsSharedPtr<TestShared>(new TestShared());
    SrsWeakPtr<TestShared>* w1 = new SrsWeakPtr<TestShared>(*s1);
    if(true) {
        SrsSharedPtr<TestShared>* lock = w1->lock();
        SrsAutoFree(SrsSharedPtr<TestShared>, lock);
        EXPECT_TRUE(lock->get()->p_ != NULL);
        EXPECT_TRUE(lock->get()->valid_);

        srs_freep(s1);

        EXPECT_TRUE(lock->get()->p_ != NULL);
        EXPECT_TRUE(lock->get()->valid_);
        EXPECT_FALSE(w1->expire());
    }

    EXPECT_TRUE(w1->expire());
    EXPECT_TRUE(NULL == w1->lock());
    srs_freep(w1);
}







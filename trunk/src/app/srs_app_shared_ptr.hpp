//
// Copyright (c) 2013-2023 The SRS Authors
//
// SPDX-License-Identifier: MIT or MulanPSL-2.0
//

#ifndef SRS_APP_SHARED_PTR_HPP
#define SRS_APP_SHARED_PTR_HPP


class SrsSharedCount
{
public:
    int s_count_;
    int w_count_;
public:
    SrsSharedCount() : s_count_(0), w_count_(0)
    {}
    virtual ~SrsSharedCount(){}

};

template<class T> class SrsWeakPtr;

template<class T>
class SrsSharedPtr
{
private:
    friend class SrsWeakPtr<T>;
    SrsSharedCount* count_;
    T* ptr_;

public:
    SrsSharedPtr(T* p)
    {
        ptr_ = p;
        count_ = new SrsSharedCount();

        // Must confirm the ptr is valible.
        if(!ptr_) {
            ptr_ = new T();
        } 

        count_->s_count_ = 1;
    }

    virtual ~SrsSharedPtr()
    {
        release();
    }

    SrsSharedPtr<T>* copy()
    {
        return new SrsSharedPtr<T>(*this);        
    }

    T* get()
    {
        return ptr_;
    }

private:
    SrsSharedPtr(){}
    SrsSharedPtr<T>& operator=(SrsSharedPtr<T> const& s)
    {}
    SrsSharedPtr(SrsSharedPtr<T> const& s)
    {
        ptr_ = s.ptr_;
        s.count_->s_count_++;
        count_ = s.count_;
    }

    SrsSharedPtr(SrsWeakPtr<T> const& w)
    {
        ptr_ = w.ptr_;
        count_ = w.count_;
        count_->s_count_++;
    }

    T& operator*()
    {
        return *ptr_;
    }

    T* operator->()
    {
        return ptr_;
    }

private:
    void release()
    {
        count_->s_count_--;
        if(count_->s_count_ < 1) {
            delete ptr_;
            if(count_->w_count_ < 1) {
                delete count_;
                count_ = NULL;
            }
        }
    }
};

template<class T>
class SrsWeakPtr
{
private:
    friend class SrsSharedPtr<T>;
    SrsSharedCount* count_;
    T* ptr_;

public:
    SrsWeakPtr(SrsSharedPtr<T>& p)
    {
        ptr_ = p.ptr_;
        count_  = p.count_;
        count_->w_count_++;
    }

    virtual ~SrsWeakPtr()
    {
        release();
    }

    SrsWeakPtr<T>* copy()
    {
        return new SrsWeakPtr<T>(*this);
    }

    SrsSharedPtr<T>* lock()
    {
        if(expire()) {
            return NULL;
        }
        return new SrsSharedPtr<T>(*this);
    }

private:
    SrsWeakPtr(){}
    SrsWeakPtr(SrsWeakPtr<T>& w)
    {
        ptr_ = w.ptr_;
        count_ = w.count_;
        count_->w_count_++;
    }

    SrsWeakPtr<T>& operator=(SrsWeakPtr<T> const& w)
    {
        if(this != &w) {
            release();
            ptr_ = w.ptr_;
            count_ = w.count_;
            count_->w_count_++;
        }
        return *this;
    }

    SrsWeakPtr<T>& operator=(SrsSharedPtr<T> const& s)
    {
        release();
        ptr_ = s.ptr_;
        count_ = s.count_;
        count_->w_count_++;
        return *this;
    }

    bool expire()
    {
        if(count_->s_count_ > 0) {
            return false;
        }
        return true;
    }

private:
    void release()
    {
        count_->w_count_--;
        if(count_->w_count_ < 1 && count_->s_count_ < 1) {
            delete count_;
            count_ = NULL;
        }
    }
};


#endif

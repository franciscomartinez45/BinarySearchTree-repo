#include <stdexcept>
#include <iostream>

using namespace std;
class null_ptr_exception : public runtime_error {
public:
    null_ptr_exception(const char* s): runtime_error(s){}
};

template <typename T>
class smart_ptr {
public:
    smart_ptr() : ptr_(nullptr), ref_(nullptr){}
    // Create a smart_ptr that is initialized to nullptr. The reference count
    // should be initialized to nullptr.

    explicit smart_ptr(T*& raw_ptr) {
        if (raw_ptr != nullptr) {
            ptr_ = raw_ptr; //point to raw_ptr
            try {
                ref_ = new int(1); //dynamically allocate a reference count
            }
            catch (bad_alloc& e) { //catch bad alloc exception
                ptr_ = nullptr; //reset pointers
                ref_ = nullptr;
            }
        }
    } 
    // Create a smart_ptr that is initialized to raw_ptr. The reference count
    // should be one. No change is made to raw_ptr.

    explicit smart_ptr(T*&& raw_ptr) {
        ptr_ = *&raw_ptr; //point to the raw pointer
        try {
            ref_ = new int(1); //dynamically allocate new memory
        }
        catch (bad_alloc& e) { //catch bad alloc exception
            delete raw_ptr; //delete the R-value 
            ptr_ = nullptr; //reset ptr_ and reference count
            ref_ = nullptr;
        }
        
    }
    //// Create a smart_ptr that is initialized to raw_ptr. The reference count
    //// should be one. If the constructor fails raw_ptr is deleted.	

    smart_ptr(const smart_ptr& rhs){
        if (rhs.ptr_ != nullptr) { //check if rhs is null, do nothing if true
            this->~smart_ptr();
            ptr_ = rhs.ptr_; //point to rhs ptr_
            ref_ = *&rhs.ref_; //point ref_ to rhs ref_ 
            *ref_ += 1; //increment the count
        }
    }
    // Copy construct a pointer from rhs. The reference count should be 
    // incremented by one.

    smart_ptr(smart_ptr&& rhs) noexcept {
       //check if this ptr_ is not already pointing to memory
        this->~smart_ptr(); //delete this memory allocations if so
        ptr_ = *&rhs.ptr_; //point to rhs ptr   
        ref_ = *&rhs.ref_; //point ref_ to rhs ref_
        rhs.ref_ = nullptr; //reset rhs pointers
        rhs.ptr_ = nullptr;
    }
    // Move construct a pointer from rhs.

    smart_ptr& operator=(const smart_ptr& rhs) {
        this->~smart_ptr(); //delete this if not already null
        ref_ = *&rhs.ref_;  //increment refence count
        *ref_ += 1;
        ptr_ = rhs.ptr_; //point ptr_ to rhs ptr
        return *this;
    }
    // This assignment should make a shallow copy of the right-hand side's
    // pointer data. The reference count should be incremented as appropriate.

    smart_ptr& operator=(smart_ptr&& rhs) noexcept {
        if (&this!=rhs){
            this->ptr_ = *&rhs.ptr_; //point to rhs ptr_
            this->ref_ = *&rhs.ref_; //point to rhs ref_ 
            rhs.ptr_ = nullptr; //reset rhs pointers
            rhs.ref_ = nullptr;
            return *this;
    }
        else {
            this->*ref -= 1;
            rhs.ptr_ = nullptr;
            rhs.ref_ = nullptr;
        }
    }
    // This move assignment should steal the right-hand side's pointer data.

    bool clone() {
        if (ptr_ == nullptr && *ref_ < 2) { //if ptr_ is null or has reference count of one
            return false; //return false
        } 
        else {
            T temp = *ptr_; //create shallow copy of ptr_ data
            this->~smart_ptr(); //delete this smart pointer
            try {
                ptr_ = new T(temp); //dynamically allocate a new T somewhere in memory
                ref_ = new int(1); //dynamically allocate new memory for reference count
            }
            catch (bad_alloc& e) { //catch a bad allococation exception for either new 
                delete ptr_; //delete any memory allocated
                delete ref_;
                ptr_, ref_ = nullptr; //set pointers to null
                return false;
            }
            return true;
        }
    }
    // If the smart_ptr is either nullptr or has a reference count of one, this
    // function will do nothing and return false. Otherwise, the referred to
    // object's reference count will be decreased and a new deep copy of the
    // object will be created. This new copy will be the object that this
    // smart_ptr points and its reference count will be one. 

    int ref_count() const {
        if (ref_ == nullptr ) { //if the ref_ count is null, return 0
            return 0;
        }
        else {
            return *ref_; //otherwise return the reference count
        }
       
    }
    // Returns the reference count of the pointed to data.

    T& operator*() {
        
      
            if (ptr_ == nullptr) { //if ptr is null
                throw null_ptr_exception("Error, accessing nullptr"); //throw a null_ptr_exception 
            }
            return *ptr_; //otherwise return reference to ptr_
       
        }
        
    
    // The dereference operator shall return a reference to the referred object.
    // Throws null_ptr_exception on invalid access. 

    T* operator->() {
        if (ptr_ == nullptr) { //If ptr is null
            throw null_ptr_exception("Error, accessing nullptr"); //throw exception 
        }
        return ptr_; //otherwise return the pointer

    }
    // The arrow operator shall return the pointer ptr_. Throws null_ptr_exception
    // on invalid access.

    ~smart_ptr() {
        if (ptr_ != nullptr) { //if pointers are not already null
            if (*ref_ > 1) { //check if the ref count is is greater than one
                *ref_-=1; //decrement the ref count
                ptr_ = nullptr; //reset pointers
                ref_ = nullptr;
                
            }
            else {
                delete ptr_, ref_; //otherwise delete those pointers
                ptr_ = nullptr;
                ref_ = nullptr;
            }
        }
    }
private:
    T* ptr_;               // pointer to the referred object
    int* ref_;             // pointer to a reference count
};



struct Point { int x = 2; int y = -5; };

int main() {
    smart_ptr<double> dsp1{ new double {3.14} };
    smart_ptr<double> dsp2, dsp3;

    dsp3 = dsp2 = dsp1;

    cout << dsp1.ref_count() << " " << dsp2.ref_count() << " " << dsp3.ref_count() << endl;
    // prints 3 3 3
    cout << *dsp1 << " " << *dsp2 << " " << *dsp3 << endl;
    // prints 3.14 3.14 3.14

    dsp1.clone();        // returns true

    cout << dsp1.ref_count() << " " << dsp2.ref_count() << " " << dsp3.ref_count() << endl;
    // prints 1 2 2
    cout << *dsp1 << " " << *dsp2 << " " << *dsp3 << endl;
    // prints 3.14 3.14 3.14
    smart_ptr<Point> sp{ new Point };
    cout << sp->x << " " << sp->y << endl;   // prints 2 -5
    int* p{ new int { 42 } };
    smart_ptr<int> sp1{p};

    cout << "Ref count is " << sp1.ref_count() << endl;	// Ref Count is 1
    {
        smart_ptr<int> sp2{ sp1 };
        cout << "Ref count is " << sp1.ref_count() << endl;	// Ref Count is 2
        cout << "Ref count is " << sp2.ref_count() << endl;	// Ref Count is 2
    }
    cout << "Ref count is " << sp1.ref_count() << endl;	// Ref Count is 1

    smart_ptr<int> sp3;

    cout << "Ref count is " << sp3.ref_count() << endl;	// Ref Count is 0

    sp3 = sp1;

    cout << "Ref count is " << sp1.ref_count() << endl;	// Ref Count is 2
    cout << "Ref count is " << sp3.ref_count() << endl;	// Ref Count is 2

    smart_ptr<int> sp4{ std::move(sp1) };

    cout << *sp4 << " " << *sp3 << endl;        // prints 42 42
    try {
        cout << *sp1 << endl;
    }
    catch(null_ptr_exception& e){
        cerr<<e.what();
    }
   
}
/********************************************************************************************
 * File: myVector.h
 * ----------------------
 * v.1 2015/11/29
 * This file exports the template class, which maintains Vector structure
 ********************************************************************************************/

#ifndef _myVector_h
#define _myVector_h

#include "error.h"
#include <iostream>
#include <stdlib.h>

/*
 * Class: MyVector<ValueType>
 * -----------------------
 */
template <typename ValueType>
class MyVector {  
/* -----------------------------------------------------------------------------------------//
 * Interface section
 * -----------------------------------------------------------------------------------------*/
public:
    /*
     * Constructor: MyVector
     * Usage: MyVector<ValueType> myVec;
     * --------------------------------
     * Initializes a new empty vector.
     */
    MyVector();

    /*
     * Destructor: ~MyVector
     * ----------------------
     * Frees any heap storage associated with this vector.
     */
    virtual ~MyVector();
    
    /*
     * Method: add
     * Usage: myVec.add(value);
     * -------------------------
     * Adds the specified value onto the end of this vector.
     */
    void add(const ValueType& value);
    
    /*
     * Method: isEmpty
     * Usage: if (myVec.isEmpty()) ...
     * -------------------------------
     * Returns true if this vector contains no elements.
     */
    bool isEmpty() const;
    
    /*
     * Method: get
     * Usage: ValueType val = myVec.get(index);
     * ----------------------------------------
     * Returns the element at the specified index in this vector.  This
     * method signals an error if the index is not in the array range.
     */
    const ValueType& get(int index) const;

    /*
     * Method: remove
     * Usage: myVec.remove(index);
     * ---------------------------
     * Removes the element at the specified index from this vector.
     * All subsequent elements are shifted one position to the left.  This
     * method signals an error if the index is outside the array range.
     */
    void remove(int index);

    /*
     * Method: removeValue
     * Usage: myVec.removeValue(value);
     * --------------------------------
     * Removes all the specified values from this vector.
     * This method signals an error if there are no elements
     * with such value in vector.
     */
    void removeValue(ValueType& value);

    /*
     * Method: size
     * Usage: int nElems = myVec.size();
     * --------------------------------
     * Returns the number of elements in this vector.
     */
    int size() const;

    /*
     * Method: isContains
     * Usage: if (myVec.isContains(value)) ...
     * ----------------------------------------
     * Returns true if this vector contains at least single such value.
     */
    bool isContains(const ValueType& value)const;

    /*
     * Deep copying support
     * --------------------
     * This copy constructor and operator= are defined to make a deep copy,
     * making it possible to pass or return vectors by value and assign
     * from one vector to another.
     */
    MyVector(const MyVector& src);
    MyVector& operator =(const MyVector& src);

    /*
     * Operator: []
     * Usage: myVec[index]
     * -------------------
     * Overloads [] to select and modificate elements from this vector. */
    ValueType& operator [](int index);
    /* Overloads [] to select and get elements from this vector
     * without modification.  */
    const ValueType& operator [](int index) const;


/* -----------------------------------------------------------------------------------------//
 * Private section
 * -----------------------------------------------------------------------------------------*/
private:
    /*
     * Implementation notes: myVector data structure
     * -------------------------------------------
     * The elements of the myVector are stored in a dynamic array of
     * the specified element type.  If the space in the array is ever
     * exhausted, the implementation doubles the array capacity.
     */

    /* Instance variables */
    ValueType* elements;        /* A dynamic array of the elements   */
    int capacity;               /* The allocated size of the array   */
    int count;                  /* The number of elements in use     */

    /* The error message function - if index is out of vector range  */
    void checkIndex(int index, int min, int max) const;
    /* Increases vector capacity  */
    void expandCapacity();
    /* Deep copy function  */
    void deepCopy(const MyVector& src);  
};


/* -----------------------------------------------------------------------------------------//
 * Implementation section
 * -----------------------
 * As the class is templated, the implementation part has
 * to be written in the same header with interface.
 * -----------------------------------------------------------------------------------------*/

/*
 * Implementation notes: Vector constructor and destructor
 * -------------------------------------------------------
 * The constructor allocates storage for the dynamic array
 * and initializes the other fields of the object.  The
 * destructor frees the memory used for the array.
 */
template <typename ValueType>
MyVector<ValueType>::MyVector() {
    capacity = 2;
    count = 0;
    elements = new ValueType[capacity];
}

template <typename ValueType>
MyVector<ValueType>::~MyVector() {
    if (elements != NULL) {
        //std::cout << "Bye-bye vec!" << std::endl;
        delete[] elements;
    }
}

/*
 * Implementation notes: add
 * -----------------------------------------
 * Controls current array capacity, and adds new elements
 * to the end of the myVector.
 */
template <typename ValueType>
void MyVector<ValueType>::add(const ValueType& value) {
    if(count == capacity)expandCapacity();
    elements[count] = value;
    count++;
}

/*
 * Implementation notes: isEmpty
 * -------------------------------
 * Returns true if this vector contains no elements.
 */
template <typename ValueType>
bool MyVector<ValueType>::isEmpty() const {
    return count == 0;
}

/*
 * Implementation notes: get
 * -----------------------------------------
 * Returns the element at the specified index in this vector.
 * Signals an error if the index is not in the array range.
 */
template <typename ValueType>
const ValueType &MyVector<ValueType>::get(int index) const {
    checkIndex(index, 0, count);
    return elements[index];
}

/*
 * Implementation notes: remove(index)
 * -----------------------------------
 * Removes the element at the specified index from this vector.
 * All subsequent elements are shifted one position to the left.  This
 * method signals an error if the index is outside the array range.
 */
template <typename ValueType>
void MyVector<ValueType>::remove(int index) {
    checkIndex(index, 0, count);
    for (int i = index; i < count - 1; i++) {
        elements[i] = elements[i + 1];//Shift elemetns in one position left
    }
    count--;
}

/*
 * Implementation notes: removeValue(value)
 * ----------------------------------------
 * Removes all the specified values from this vector.
 * This method signals an error if there are no elements
 * with such value in vector.
 */
template <typename ValueType>
void MyVector<ValueType>::removeValue(ValueType& value) {
    if(!isContains(value)){error("remove: Attempting to remove not existing element");
    }else{
        for (int i = 0; i < count; i++) {
            if(elements[i] == value){
                remove(i);
            }
        }
    }
}

/*
 * Implementation notes: size
 * -------------------------------
 * Returns the number of elements in this vector.
 */
template <typename ValueType>
int MyVector<ValueType>::size() const {
    return count;
}

/*
 * Implementation notes: isContains
 * --------------------------------
 * Returns true if this vector contains at least single such value.
 */
template <typename ValueType>
bool MyVector<ValueType>::isContains(const ValueType& value)const{
    bool contains = false;
    for(int i = 0; i < count; i++){
        if(elements[i] == value){
            contains = true;
            break;
        }
    }
    return contains;
}

/*
 * Implementation notes: copy constructor
 * --------------------------------------
 * The copy constructor provides deep copying to pass MyVector
 * objects by value, or to initiate new objects.
 */
template <typename ValueType>
MyVector<ValueType>::MyVector(const MyVector& src) {
    deepCopy(src);
}

/*
 * Implementation notes: assignment operator
 * -----------------------------------------
 * Provides deep copying to assign one existed MyVector object
 * to another.
 */
template <typename ValueType>
MyVector<ValueType> & MyVector<ValueType>::operator =(const MyVector& src) {
    if (this != &src) {
        if (elements != NULL) {
            delete[] elements;
        }
        deepCopy(src);
    }
    return *this;
}

/*
 * Implementation notes: Vector [] selection
 * -----------------------------------------
 * Next two methods implement traditional array selection using
 * square brackets for the index.
 * Overloads [] to select and modificate elements from this vector.
 */
template <typename ValueType>
ValueType& MyVector<ValueType>::operator [](int index) {
    checkIndex(index, 0, count-1);
    return elements[index];
}

/* Overloads [] to select and get elements from this
 * vector without modification.  */
template <typename ValueType>
const ValueType& MyVector<ValueType>::operator [](int index) const {
    checkIndex(index, 0, count-1);
    return elements[index];
}

/*
 * Private function: checkIndex()
 * -----------------------------------------
 * The error message function - if index is out of vector range.
 */
template <typename ValueType>
void MyVector<ValueType>::checkIndex(int index, int min, int max) const {
    if (index < min || index > max) {error("MyVector:: index is outside of valid range");  }
}

/*
 * Private function: expandCapacity()
 * -----------------------------------------
 * Doubles the array capacity, copies the old elements
 * into the new array, and then frees the old one.
 */
template <typename ValueType>
void MyVector<ValueType>::expandCapacity() {
    ValueType *oldArray = elements;
    capacity = 2 * capacity;
    elements = new ValueType[capacity];
    for(int i = 0; i < count; i++){
        elements[i] = oldArray[i];
    }
    delete []oldArray;
}

/*
 * Private function: deepCopy()
 * -----------------------------------------
 * Copies data from src param into the current object.
 * All dynamic memory is reallocated to create a "deep copy"
 * of param array.
 */
template <typename ValueType>
void MyVector<ValueType>::deepCopy(const MyVector& src) {
    count = capacity = src.count;
    elements = (capacity == 0) ? NULL : new ValueType[capacity];
    for (int i = 0; i < count; i++) {
        elements[i] = src.elements[i];//Coping process
    }
}


#endif

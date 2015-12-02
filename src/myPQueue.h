/********************************************************************************************
 * File: myPQueue.h
 * ----------------------
 * v.1 2015/11/29
 * This file exports the template class, which maintains
 * structure for priority queue creation.
 ********************************************************************************************/

#ifndef _myPQueue_h
#define _myPQueue_h

#include "error.h"
#include <iostream>
#include <stdlib.h>

/*
 * Class: MyPQueue<ValueType>
 * -----------------------
 * Implements priority queue structure
 * with linked list based implementation.
 * MyPQueue class suport only descending priority order -
 * value with smaller priority is dequeued first.
 */
template <typename ValueType>
class MyPQueue {
/* -----------------------------------------------------------------------------------------//
 * Interface section
 * -----------------------------------------------------------------------------------------*/
public:
    /*
     * Constructor: MyPQueue
     * Usage: MyPQueue<ValueType> mpq;
     * ------------------------------
     * Initializes a new empty queue.
     */
    MyPQueue();

    /*
     * Destructor: ~MyPQueue
     * ----------------------
     * Frees any heap storage associated with this queue.
     */
    virtual ~MyPQueue();
    
    /*
     * Method: size
     * Usage: int nElems = myPQueue.size();
     * ------------------------------------
     * Returns the number of elements in priority queue.
     */
    int size() const;

    /*
     * Method: isEmpty
     * Usage: if (mpq.isEmpty()) ...
     * -------------------------------
     * Returns true if this priority queue contains no elements.
     */
    bool isEmpty() const;

    /*
     * Method: enqueue
     * Usage: mpq.enqueue(value, 1);
     * -----------------------------
     * Adds the specified value into the priority queue. New value
     * will be closer to the output from the queue if it's
     * priority will be less, but not negative. The oldest values,
     * of the same priority, aren't moved - so FIFO rule is work.
     */
    void enqueue(const ValueType& value, const int priority);

    /*
     * Method: peek
     * Usage: mpq.peek();
     * -----------------------------
     * Returns, but does not remove, the closiest to the output
     * from the queue value.
     */
    ValueType peek() const;

    /*
     * Method: dequeueMin
     * Usage: mpq.dequeueMin();
     * -----------------------------
     * Returns, and removes from the queue, the smallest priority value.
     */
    ValueType dequeueMin();

    /*
     * Method: copy constructor
     * --------------------------------------
     * The copy constructor provides deep copying to pass MyPQueue
     * objects by value, or to initiate new objects.
     */
    MyPQueue(const MyPQueue<ValueType> &src);

    /*
     * Method: assignment operator
     * -----------------------------------------
     * Provides deep copying to assign one existed MyPQueue object
     * to another.
     */
    MyPQueue& operator =(const MyPQueue& src);

/* -----------------------------------------------------------------------------------------//
 * Private section
 * -----------------------------------------------------------------------------------------*/
private:
    /*
     * Implementation notes: MyPQueue internal structure
     * -------------------------------------------------
     * The elements of the MyPQueue are stored in the linked list of internal
     * Cell structures. Each Cell is a single storage element for single
     * user value and priority. Each Cell is placed in memory heap. Connection
     * between cells is realised by unidirected links to next cell. Along
     * the main list cells are placed due to their priorities - to be
     * dequeued first, element has to be enqueued first, and to have the smallest
     * priority.
     *
     * Another main element of the class is head - it points to the first element
     * to dequeue from queue.
     */

    /* Instance variables */

    /* Type: Cell
     * -------------------------------------------------
     * Main storage object for user values. The chain of
     * cells, combined in linked list, create main queue
     * storage structure - linked list. */
    struct Cell{
        /* Instance fields */
        int priority;       /* User priority for this data value  */
        ValueType data;     /* User data value  */
        Cell * link;        /* Link to the next cell in the queue list  */

        bool operator==(const Cell & n1){
                 return (priority == n1.priority) && (data == n1.data) && (link == n1.link);
        }
    };

    Cell * head;            /* Pointer to the first element to dequeue from queue.  */
    int count;              /* Enqueued elements counter.  */

    /* Private methods    */

    /*
     * Private method: clear
     * ------------------------
     * Removes all elements from queue
     * due to service needs.
     */
    void clear();

    /*
     * Private method: deepCopy
     * ------------------------
     * Implements deep copy process.
     */
    void deepCopy(const MyPQueue<ValueType>& src);
};


/* -----------------------------------------------------------------------------------------//
 * Implementation section
 * -----------------------
 * As the class is templated, the implementation part has
 * to be written in the same header with interface.
 * -----------------------------------------------------------------------------------------*/

/*
 * Implementation notes: myPQueue constructor and destructor
 * -------------------------------------------------------
 * The constructor allocates storage for the dynamic array
 * and initializes the other fields of the object.  The
 * destructor frees the memory used for the array.
 */
template <typename ValueType>
MyPQueue<ValueType>::MyPQueue() {
    head = NULL;
    count = 0;
}

template <typename ValueType>
MyPQueue<ValueType>::~MyPQueue() {
    clear();
}

/*
 * Implementation notes: size
 * ------------------------------------
 * Returns value of enqueuings counter.
 */
template <typename ValueType>
int MyPQueue<ValueType>::size() const {
    return count;
}

/*
 * Implementation notes: isEmpty
 * -------------------------------
 * Returns true if enqueuings counter is thero.
 */
template <typename ValueType>
bool MyPQueue<ValueType>::isEmpty() const {
    return count == 0;
}

/*
 * Implementation notes: enqueue
 * -----------------------------
 * Method surf through the list to find suitable
 * place for such input data priority - looks for cell,
 * wich priority bigger then input priority. Also
 * it remembers last looked cell.
 * When searching is finished it writes the cell with higher
 * priority as a link for input cell, and rewrites input cell
 * as a link to previous cell - which priority isn't bigger.
 */
template <typename ValueType>
void MyPQueue<ValueType>::enqueue(const ValueType& value, const int priority) {
    if(priority < 0)error("enqueue: Attempting to enqueue negative priority");
    Cell * inputCell = new Cell;
    inputCell->data = value;
    inputCell->priority = priority;
    inputCell->link = NULL;
    if(head == NULL){
        head = inputCell;//It is first input into the queue.
    }else if(priority < head->priority){
        inputCell->link = head;//Move old head cell inside queue.
        head = inputCell;      //Put this input cell as a head.
    }else{
        Cell * cp = head;//Marker, which priority will be bigger then input cell priority.
        Cell * previousCell = head;//Marker, which priority will be even or less then input priority.
        while(priority >= cp->priority){//The oldest values are passed - so FIFO rule is work.
            previousCell = cp;//Move deeper into queue to find suitable place
            cp = cp->link;
            if(cp == NULL)break;//At this condition inputCell will be end of list
        }
        /* Main insertion process */
        previousCell->link = inputCell;
        inputCell->link = cp;
    }
    count++;
}

/*
 * Implementation notes: peek
 * -----------------------------
 * Return the head cell data.
 */
template <typename ValueType>
ValueType MyPQueue<ValueType>::peek() const{
    if(isEmpty())error("peek: Attempting to peek empty queue");
    return head->data;
}

/*
 * Implementation notes: dequeueMin
 * --------------------------------
 * Reliefes current head cell, and move head on the next cell.
 */
template <typename ValueType>
ValueType MyPQueue<ValueType>::dequeueMin(){
    if(isEmpty())error("dequeueMin: Attempting to dequeue from empty queue");
    Cell *cp = head;
    ValueType result = cp->data;
    head = cp->link;
    delete cp;
    count--;
    return result;
}

/*
 * Implementation notes: copy constructor and assignment operator
 * --------------------------------------------------------------
 * The constructor and assignment operators follow a standard paradigm
 * of deep copying.
 */
template <typename ValueType>
MyPQueue<ValueType>::MyPQueue(const MyPQueue<ValueType>& src) {
    deepCopy(src);
}

template <typename ValueType>
MyPQueue<ValueType> & MyPQueue<ValueType>::operator =(const MyPQueue& src) {
    std::cout << "Hey! Assign!" << std::endl;
    if (this != &src) {
        clear();
        deepCopy(src);
    }
    return *this;
}

/*
 * Private function: clear
 * ------------------------
 * Removes all elements from queue
 */
template <typename ValueType>
void MyPQueue<ValueType>::clear(){
    while(count > 0){
        dequeueMin();
    }
}

/*
 * Private function: deepCopy
 * -----------------------------------------
 * Copies data from src param into the current object.
 * All dynamic memory is reallocated to create a "deep copy"
 * of param array. Erases all current internal  fields.
 */
template <typename ValueType>
void MyPQueue<ValueType>::deepCopy(const MyPQueue<ValueType>& src) {
    head = NULL;
    count = 0;
    for (Cell *cp = src.head; cp != NULL; cp = cp->link) {
        enqueue(cp->data, cp->priority);
    }
    //std::cout << "DeepCopied!" << std::endl;
}


#endif

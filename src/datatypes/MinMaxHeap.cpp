//------------------------------------------------------------------------
// File:     MinMaxHeap.C
// Project:  CMSC 341  Project 5  Fall 2003
// Author:   Todd Wease, Section 0301
// Created:  11/24/03
// Current:  12/05/03
// 
// The min-max heap is an extension of the binary heap that has both a
// min order and max order thus supporting both deleteMin and deleteMax.
// Min order of the heap is maintained on even levels of the heap, the
// root being at level 0, and max order is maintained on odd levels of the
// heap.  Both orders are intermeshed such that no child of a min level
// is ever less than the parent and no child of a max level is ever greater
// than the parent.
//   
//-------------------------------------------------------------------------
// I have read and I understand the course policy on cheating. By submitting
// the following program, I am stating that the program was produced by my
// individual effort. 
//-------------------------------------------------------------------------

#ifndef MINMAXHEAP_C
#define MINMAXHEAP_C

#include <iostream>
#include "MinMaxHeap.hpp"
#include "../base/is_exception.hpp"

// Constructor
// Parameter capacity: capacity of the min-max heap
template <class Comparable>
MinMaxHeap<Comparable>::MinMaxHeap( int capacity )
   : _currentSize( 0 ), _array( capacity + 1 )
{
   // no code
}

// FindMin
// Finds and returns the minimum item in the heap
// Returns: minimum item in the heap
template <class Comparable>
const Comparable & MinMaxHeap<Comparable>::findMin() const
{
  if ( isEmpty() )
    D_throw() << "*** FindMin failed: Heap is empty ***";
  
   return _array[ 1 ];
}

// FindMax
// Finds and returns the maximum item in the heap
// Returns: maximum item in the heap
template <class Comparable>
const Comparable & MinMaxHeap<Comparable>::findMax() const
{
   if ( isEmpty() )
     D_throw() << "*** FindMax failed: Heap is empty ***";

   if ( _currentSize == 1 )
      return _array[ 1 ];
   else if ( _currentSize == 2 )
      return _array[ 2 ];
   else
      return _array[ 2 ] > _array[ 3 ] ? _array[ 2 ] : _array[ 3 ];
}

// Insert
// Inserts item into the min-max heap, maintaining heap order
//   Duplicates are allowed.
// Parameter x: item to be inserted
// Side Effects: throws Overflow if heap is full
template <class Comparable>
void MinMaxHeap<Comparable>::insert( const Comparable & x )
{
   if( isFull() )
     D_throw() << "*** Insert failed: Heap is full ***";

   int hole = ++_currentSize;
   _array[ hole ] = x;

   percolateUp( hole );
}

// PercolateUp
// Used to maintain min-max heap order after insertion.
//   Determines whether current heap level is a min level
//   or a max level and calls percolateUpMin or percolateUpMax.
// Parameter hole: index in array where item was inserted
template <class Comparable>
void MinMaxHeap<Comparable>::percolateUp( int hole )
{
   int parent = hole / 2;
   int level = 0;

   for ( int i = 2; i <= hole; i *= 2 )
      level++;

   // min level
   if ( level % 2 == 0 ) {

      if ( parent > 0 && _array[ hole ] > _array[ parent ] ) {
         swap( hole, parent );
         percolateUpMax( parent );
      }
      else
         percolateUpMin( hole );
   }
   // max level
   else {

      if ( parent > 0 && _array[ hole ] < _array[ parent ] ) {
         swap( hole, parent );
         percolateUpMin( parent );
      }
      else
         percolateUpMax( hole );
   }
}

// PercolateUpMin
// Called by percolateUp to maintain min-max heap order on 
//   the min levels.
// Parameter hole: index in array of item to be percolated
template <class Comparable>
void MinMaxHeap<Comparable>::percolateUpMin( int hole )
{
   int grandparent = hole / 4;

   if ( grandparent > 0 && _array[ hole ] < _array[ grandparent ] ) {
      swap( hole, grandparent );
      percolateUpMin( grandparent );
   }
}

// PercolateUpMax
// Called by percolateUp to maintain min-max heap order on
//   the max levels.
// Parameter hole: index in array of item to be percolated
template <class Comparable>
void MinMaxHeap<Comparable>::percolateUpMax( int hole )
{
   int grandparent = hole / 4;

   if ( grandparent > 0 && _array[ hole ] > _array[ grandparent ] ) {
      swap( hole, grandparent );
      percolateUpMax( grandparent );
   }
}

// DeleteMin
// Remove the smallest item from the min-max heap and
//   place it in minItem.
// Parameter minItem: reference from calling function
//   to put the smallest item in.
// Side Effects: throws Underflow if heap is empty
template <class Comparable>
void MinMaxHeap<Comparable>::deleteMin( Comparable & minItem )
{
   if( isEmpty() )
     D_throw() << "*** DeleteMin failed: Heap is empty ***";

   minItem = _array[ 1 ];
   _array[ 1 ] = _array[ _currentSize-- ];
   percolateDown( 1 );
}

// DeleteMax
// Remove the largest item from the min-max heap and
//   place it in maxItem.
// Parameter maxItem: reference from calling function
//   to put the largest item in.
// Side Effects: throws Underflow if heap is empty
template <class Comparable>
void MinMaxHeap<Comparable>::deleteMax( Comparable & maxItem )
{
   int maxIndex;

   if ( isEmpty() )
     D_throw() << "*** DeleteMax failed: Heap is empty ***";

   if ( _currentSize == 1 )
      maxIndex = 1;
   else if ( _currentSize == 2 )
      maxIndex = 2;
   else
      maxIndex = _array[ 2 ] > _array[ 3 ] ? 2 : 3;

   maxItem = _array[ maxIndex ];
   _array[ maxIndex ] = _array[ _currentSize-- ];
   percolateDown( maxIndex );

}

// PercolateDown
// Used to maintain min-max heap order after deletion.
//   Determines whether current heap level is a min level
//   or a max level and calls percolateDownMin or percolateDownMax.
// Parameter hole: index in array where item was deleted
template <class Comparable>
void MinMaxHeap<Comparable>::percolateDown( int hole )
{
   int level = 0;

   for ( int i = 2; i <= hole; i *= 2 )
      level++;

   if ( level % 2 == 0 )
      percolateDownMin( hole );
   else
      percolateDownMax( hole );
}

// PercolateDownMin
// Called by percolateDown to maintain min-max heap order on
//   the min levels.
// Parameter hole: index in array of item to be percolated
template <class Comparable>
void MinMaxHeap<Comparable>::percolateDownMin( int hole )
{
   int minIndex;

   // find minimum value of children and grandchildren
   // hole * 2 = index of first child if it exists
   // hole * 4 = index of first grandchild if it exists
   minIndex = findMinDescendent( hole * 2, hole * 4 );

   // at least one descendent
   if ( minIndex > 0 ) {

      // min descendent is a grandchild
      if ( minIndex >= hole * 4 ) {

         // if less than grandparent, i.e value at hole, swap
         if ( _array[ minIndex ] < _array[ hole ] ) {
            swap( hole, minIndex );

            // if greater than parent, swap
            if ( _array[ minIndex ] > _array[ minIndex / 2 ] )
               swap( minIndex, minIndex / 2 );

            percolateDownMin( minIndex );
         }
      }
      // min descendent is a child
      else {

         // if less than parent, i.e value at hole, swap
         if ( _array[ minIndex ] < _array[ hole ] )
            swap( hole, minIndex );
      }
   }
}

// PercolateDownMax
// Called by percolateDown to maintain min-max heap order on
//   the max levels.
// Parameter hole: index in array of item to be percolated
template <class Comparable>
void MinMaxHeap<Comparable>::percolateDownMax( int hole )
{
   int maxIndex;

   // find maximum value of children and grandchildren
   // hole * 2 = index of first child if it exists
   // hole * 4 = index of first grandchild if it exists
   maxIndex = findMaxDescendent( hole * 2, hole * 4 );

   // at least one descendent
   if ( maxIndex > 0 ) {

      // max descendent is a grandchild
      if ( maxIndex >= hole * 4 ) {

         // if greater than grandparent, i.e value at hole, swap
         if ( _array[ maxIndex ] > _array[ hole ] ) {
            swap( hole, maxIndex );

            // if less than parent, swap
            if ( _array[ maxIndex ] < _array[ maxIndex / 2 ] )
               swap( maxIndex, maxIndex / 2 );

            percolateDownMax( maxIndex );
         }
      }
      // max descendent is a child
      else {

         // if greater than parent, i.e value at hole, swap
         if ( _array[ maxIndex ] > _array[ hole ] )
            swap( hole, maxIndex );
      }
   }
}

// FindMinDescendent
// Helper function of percolateDownMin that finds the min
//   of the children and grandchildren of the item to be
//   percolated down
// Parameter child: array index of first child
// Parameter grandchild: array index of first grandchild
template <class Comparable>
int MinMaxHeap<Comparable>::findMinDescendent( int child, int grandchild )
{
   int minIndex = 0;
   int minChild = child;
   int minGrandchild = grandchild;

   if ( child <= _currentSize ) {

      if ( child != _currentSize && _array[ child + 1 ] < _array[ minChild ] )
         minChild = child + 1;

      minIndex = minChild;

      if ( grandchild <= _currentSize ) {

         for ( int i = 1; grandchild < _currentSize && i < 4; i++, grandchild++ ) {

            if ( _array[ grandchild + 1 ] < _array[ minGrandchild ] )
               minGrandchild = grandchild + 1;
         }

         if ( _array[ minGrandchild ] < _array[ minChild ] )
            minIndex = minGrandchild;
      }
   }

   return minIndex;
}

// FindMaxDescendent
// Helper function of percolateDownMax that finds the max
//   of the children and grandchildren of the item to be
//   percolated down
// Parameter child: array index of first child
// Parameter grandchild: array index of first grandchild
template <class Comparable>
int MinMaxHeap<Comparable>::findMaxDescendent( int child, int grandchild )
{
   int maxIndex = 0;
   int maxChild = child;
   int maxGrandchild = grandchild;

   if ( child <= _currentSize ) {

      if ( child != _currentSize && _array[ child + 1 ] > _array[ maxChild ] )
         maxChild = child + 1;

      maxIndex = maxChild;

      if ( grandchild <= _currentSize ) {

         for ( int i = 1; grandchild < _currentSize && i < 4; i++, grandchild++ ) {

            if ( _array[ grandchild + 1 ] > _array[ maxGrandchild ] )
               maxGrandchild = grandchild + 1;
         }

         if ( _array[ maxGrandchild ] > _array[ maxChild ] )
            maxIndex = maxGrandchild;
      }
   }

   return maxIndex;
}

// Swap
// Swaps elements of two indices
// Parameter indexOne: first index of array of item to be swapped
// Parameter indexTwo: second index of array of item to be swapped
template <class Comparable>
void MinMaxHeap<Comparable>::swap( int indexOne, int indexTwo )
{
   Comparable tmp = _array[ indexOne ];
   _array[ indexOne ] = _array[ indexTwo ];
   _array[ indexTwo ] = tmp;
}

// IsEmpty
// Checks to see if heap is empty
// Returns: true if empty, false if not
template <class Comparable>
bool MinMaxHeap<Comparable>::isEmpty() const
{
   return _currentSize == 0;
}

// IsFull
// Checks to see if heap is logically full
// Returns: true if full, false if not
template <class Comparable>
bool MinMaxHeap<Comparable>::isFull() const
{
   return _currentSize == ( int )_array.size() - 1;
}

// Print
// Prints contents of min-max heap by printing
//   each element of the underlying array from
//   root( index 1 ) to the last index
// Parameter out: the output stream to print to
template <class Comparable>
void MinMaxHeap<Comparable>::print( ostream & out )
{
   if ( !isEmpty() ) {

      for ( int i = 1; i <= _currentSize; i++ )
         out << _array[ i ] << endl;
   }
   else
     D_throw() << "HEAP IS EMPTY";
}

#endif


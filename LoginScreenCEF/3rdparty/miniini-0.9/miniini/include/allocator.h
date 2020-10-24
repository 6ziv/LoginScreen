// Copyright (C) 2009-2010 Ferdinand Majerech
// This file is part of MiniINI
// For conditions of distribution and use, see copyright notice in LICENSE.txt


#ifndef ALLOCATOR_H_INCLUDED
#define ALLOCATOR_H_INCLUDED

#include "typedefs.h"
#include "miniini_assert.h"

namespace miniini_private
{
/// @cond PRIVATE
    
///Single block of allocated memory in Allocator.
class Block
{
    ///Allocator has direct access so it can work with Block as fast as possible
    friend class Allocator;
    private:
        ///Amount of allocated memory in bytes.
        ui Allocated;
        ///Amount of used memory in this block, in bytes.
        ui Used;
        ///Amount of pointers to memory given to outside code by this block.
        ui PtrsGiven;
        ///Amount of pointers to memory in this block that were deleted.
        ui PtrsDeleted;
        ///Allocated memory of the block.
        c * Data;

    public:
        /** Constructor.
         * Allocates memory for this block.
         * @param alloc Amount of memory to allocate
         */
        Block(const ui alloc)
            :Allocated(alloc)
            ,Used(0)
            ,PtrsGiven(0)
            ,PtrsDeleted(0)
            ,Data(new c [Allocated])
        {}
        
        ///Destructor. Deletes memory of this block.
        ~Block()
        {
            delete [] Data;
        }

        ///@return free space in this block
        ui GetRemainingSpace() const
        {
            return Allocated - Used;
        }

        /** Reallocates this block. 
         * Can only be called before the first time this block is used.
         * @param size Amount of memory to reallocate this block to.
         */
        void Reallocate(const ui size)
        {
            //This can only be called before this block is used for the first time.
            MINIINI_ASSERT(!(Used || PtrsGiven || PtrsDeleted),
                           "Block::Reallocate called on block that was already"
                           "used.");
            Allocated = size;
            delete [] Data;
            Data = new c [Allocated];
        }

    private:

        ///No copy construction or assignment.

        Block(const Block &);

        void operator = (const Block &);
};

/** Allocates memory for character data in an INIFile. 
 * Memory is allocated and stored in large blocks.
 */
class Allocator
{
    private:
        ///Number of blocks currently used.
        ui NumBlocks;
        
        /** Block we're currently allocating memory from. 
         * When the first block gets filled, we start taking memory from the
         * second one, and so on.
         */
        ui CurrentBlock;

        ///Minimum size of a block (used when allocating new blocks).
        ui BlockMinSize;

        ///Pointers to blocks of allocated memory.
        Block * * Blocks;

    public:
        /** Constructor. 
         * Allocates given amount of memory divided into given number of blocks.
         * @param size Amount of memory to allocate.
         * @param blocks Number of blocks to divide memory to.
         */
        Allocator(const ui size, const ui blocks);
        
        ///Removes unused blocks left after construction.
        void Trim();

        /** Deletes space pointed to by given pointer.
         * Equivalent to delete [] or free()
         * @param ptr Pointer to space to delete. 
         */
        void DeleteSpace(c * const ptr);

        /** Allocates space with specified size and returns a pointer to it.
         * @param size Amount of space to allocate in bytes.
         * @return pointer to allocated space.
         */
        c * NewSpace(const ui size);

        ///Destructor. Deallocates any blocks left.
        ~Allocator();

    private:
        
        /** Move to next block. If no more blocks exist, one will be allocated.
         * @param minsize Minimum size the next block must have (if it's smaller, it'll get reallocated)
         */
        void NextBlock(ui minsize);
        
        ///Allocate a new block.
        void NewBlock();

        /** Delete block with given index.
         * @param index Index of the block to delete.
         */
        void DeleteBlock(ui index);

        /** Finds the block to which the given pointer points.
         * @param ptr Pointer to block to find.
         * @return -1 if ptr doesn't point to any block.
         * @return index to block to which ptr points if successful.
         */
        i FindBlock(c * const ptr);

        //No copy construction or assignment.

        Allocator(const Allocator &);

        void operator = (const Allocator &);
};
/// @endcond

}
#endif

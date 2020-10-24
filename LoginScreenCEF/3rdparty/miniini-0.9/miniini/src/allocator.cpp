// Copyright (C) 2009-2010 Ferdinand Majerech
// This file is part of MiniINI
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include <cstring>

#include "allocator.h"
#include "time.h"
#include "globals.h"

namespace miniini_private
{

Allocator::Allocator(const ui size, const ui blocks)
    :NumBlocks(blocks)
    ,CurrentBlock(0)
    ,BlockMinSize(size / NumBlocks + 1)
    ,Blocks(new Block * [NumBlocks])
{
    #ifdef MINIINI_BENCH_EXTRA
    #ifdef linux
    ld atimestart = GetTime();
    #endif
    #endif
    //Allocate enough blocks to have size of space
    for(ui block = 0; block < NumBlocks; ++block)
    {
        Blocks[block] = new Block(BlockMinSize);
    }
    #ifdef MINIINI_BENCH_EXTRA
    #ifdef linux
    bench_alloctime += GetTime() - atimestart;
    #endif
    #endif
}

void Allocator::Trim()
{
    //Since current block can only be incremented,
    //if the current block is not the last block, there are
    //some blocks left over from constructor that we can delete.
    while(CurrentBlock < (NumBlocks - 1))
    {
        DeleteBlock(CurrentBlock + 1);
    }
}

void Allocator::DeleteSpace(c * const ptr)
{
    i blockidx = FindBlock(ptr);
    //ptr must point to some block. Otherwise we have an error
    MINIINI_ASSERT(blockidx >= 0, "Pointer that doesn't point to space "
                                  "allocated by this instance of Allocator "
                                  "was passed as pointer to space to delete "
                                  "to Allocator::DeleteSpace()");
    Block * block = Blocks[blockidx];
    ++block->PtrsDeleted;
    //If as many pointers as given out were deleted, this block is no
    //longer used and we can delete it
    if(block->PtrsDeleted == block->PtrsGiven)
    {
        //This is the current block: no need to delete it as it's not yet full
        if(static_cast<ui>(blockidx) == CurrentBlock)
        {
            return;
        }
        DeleteBlock(blockidx);
    }
    return;
}

c * Allocator::NewSpace(const ui size)
{
    Block * block = Blocks[CurrentBlock];
    //Not enough space in this block. Add new block and allocate from there
    if((block->Allocated - block->Used) < size)
    {
        NextBlock(size);
        return NewSpace(size);
    }
    //Pointer to allocated space
    c * out = block->Data + block->Used;
    //Update block data
    block->Used += size;
    ++block->PtrsGiven;
    return out;
}

Allocator::~Allocator()
{
    //Delete all blocks
    for(ui block = 0; block < NumBlocks; ++block)
    {
        delete Blocks[block];
    }
    delete [] Blocks;
}

void Allocator::NextBlock(const ui minsize)
{
    ++CurrentBlock;
    //Size of the new block is max(minsize, BlockMinSize)
    if(minsize > BlockMinSize)
    {
        BlockMinSize = minsize;
    }
    //Not the last block, we have more allocated blocks so we just need to move 
    //to the next one
    if(CurrentBlock < NumBlocks)
    {
        //If the block is not large enough, enlarge it
        if(Blocks[CurrentBlock]->GetRemainingSpace() < minsize)
        {
            Blocks[CurrentBlock]->Reallocate(BlockMinSize);
        }
    }
    //We need to allocate a new block (and reallocate the Blocks ptr array)
    else
    {
        NewBlock();
    }
}

void Allocator::NewBlock()
{
    #ifdef MINIINI_BENCH_EXTRA
    #ifdef linux
    ld atimestart = GetTime();
    #endif
    #endif
    Block * * tempblocks = new Block * [NumBlocks + 1];
    memcpy(tempblocks, Blocks, NumBlocks * sizeof(Block *));
    tempblocks[NumBlocks] = new Block(BlockMinSize);
    delete [] Blocks;
    Blocks = tempblocks;
    ++NumBlocks;
    #ifdef MINIINI_BENCH_EXTRA
    #ifdef linux
    bench_alloctime += GetTime() - atimestart;
    #endif
    #endif
}

void Allocator::DeleteBlock(ui index)
{
    MINIINI_ASSERT(index < NumBlocks, "Block index passed to "
                                      "Allocator::DeleteBlock() out of range");
    Block * const block = Blocks[index];
    NumBlocks -= 1;
    delete block;
    //This is the only block left, so delete it and replace by an empty block.
    if(!NumBlocks)
    {
        //delete block;
        NewBlock();
        return;
    }

    //Reallocate Blocks to smaller size
    Block * * tempblocks = new Block * [NumBlocks];
    memcpy(tempblocks, Blocks, index * sizeof(Block *));
    memcpy(tempblocks + index, Blocks + index + 1, 
           (NumBlocks - index) * sizeof(Block *));
    delete [] Blocks;
    Blocks = tempblocks;
}

i Allocator::FindBlock(c * const ptr)
{
    for(ui blockidx = 0; blockidx < NumBlocks; ++blockidx)
    {
        Block * block = Blocks[blockidx];
        if(ptr >= block->Data && ptr < (block->Data + block->Allocated))
        {
            return blockidx;
        }
    }
    return -1;
}

}

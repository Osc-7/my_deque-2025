
# My sjtu::deque 2025

## Project Overview

This project implements a deque (double-ended queue) using an unrolled linked list data structure. The implementation provides efficient random access while maintaining O(1) amortized time complexity for head/tail operations and O(√n) worst-case time complexity for random access operations.

## Key Features

- **Dynamic block sizing**: Blocks automatically split and merge to maintain optimal size
- **Efficient operations**:
  - O(1) amortized push/pop at both ends
  - O(√n) worst-case random access
- **Memory efficient**: Blocks are merged when underutilized
- **Cache-friendly**: Block-based storage improves data locality

## Implementation Strategy

### Block Management

#### Block Size Parameters
The deque maintains blocks with sizes dynamically adjusted based on √n scaling:

```cpp
size_t min_block_size() const {
  return std::max(size_t(100), (size_t)std::sqrt(total_size) / 3);
}

size_t max_block_size() const {
  return std::max(size_t(200), (size_t)std::sqrt(total_size));
}
```

These parameters ensure that:
1. The number of blocks remains O(√n)
2. Each block contains O(√n) elements
3. Operations stay within required time complexities
4. Minimum sizes prevent excessive splitting for small deques

#### Splitting Strategy
When a block exceeds max_block_size():
1. Create new block with half the elements (O(k) where k ≤ √n)
2. Insert new block after current block (O(1) linked list operation)

```cpp
void split_block(iterator it) {
  if (it->data.size() <= max_block_size()) return;
  
  // Split logic: O(k) operation
  auto split_pos = /* calculate middle position */;
  new_block.splice(new_block.begin(), it->data, split_pos, it->data.end());
  
  blocks.insert(++it, new_block); // O(1)
}
```

#### Merging Strategy
When adjacent blocks could combine within max_block_size():
1. Move all elements from next block (O(k) where k ≤ √n)
2. Remove the empty block (O(1) linked list operation)

```cpp
void merge_blocks(iterator it) {
  if (next_it->data.size() + it->data.size() > max_block_size()) return;
  
  // Merge logic: O(k) operation
  it->data.splice(it->data.end(), next_it->data);
  blocks.erase(next_it); // O(1)
}
```

### Time Complexity Analysis

| Operation               | Time Complexity | Reasoning |
|-------------------------|-----------------|-----------|
| **push_front/push_back** | O(1) amortized  | - Direct access to head/tail blocks<br>- Split cost O(√n) amortized over Ω(√n) operations |
| **pop_front/pop_back**  | O(1) amortized  | - Immediate removal from ends<br>- Merge cost O(√n) amortized over Ω(√n) operations |
| **random access []**     | O(√n) worst-case | - At most O(√n) blocks to traverse<br>- At most O(√n) elements to scan in target block |
| **insert/erase**         | O(√n) worst-case | - Position finding: O(√n)<br>- Potential split/merge: O(√n) |

## Why the Strategy Works

1. **Block Size Maintenance**:
   - Minimum size (√n/3) prevents excessive fragmentation
   - Maximum size (√n) ensures no block becomes too large
   - Results in O(√n) total blocks containing all n elements

2. **Amortized O(1) End Operations**:
   - Splitting/merging costs O(√n) but occurs only every Ω(√n) operations
   - Average cost becomes O(1) per operation
   - Example: Splitting a block of size 2√n into two √n blocks won't need splitting again until √n more inserts

3. **O(√n) Random Access**:
   - Binary search through O(√n) blocks possible (though not implemented)
   - Linear scan within block limited to O(√n) elements
   - Combined cost remains sublinear

4. **Memory Efficiency**:
   - Merging underutilized blocks reclaims wasted space
   - No block remains smaller than √n/3 (except possibly the last block)

This balanced approach maintains optimal performance across all operations while dynamically adapting to the deque's current size, achieving both good theoretical complexity and practical cache efficiency.
# HashMap Algorithms: Robin Hood Hashing, Arena Allocation, and a HashSet for Free

This document explains the algorithms used in the core hash map and its set variant. It is a standalone guide, not API reference.

The goal is to explain not just what the code does, but why this particular design fits a low-level, arena-backed systems codebase.

## The Problem We Want to Solve

We want a hash container with these properties:

- fast lookups and inserts;
- no per-node heap allocation;
- predictable memory layout;
- compatibility with the project's arena allocator;
- support for both key-value maps and key-only sets;
- explicit error handling through `Result<E,T>`.

That combination points strongly toward open addressing rather than separate chaining.

Separate chaining would introduce pointer chasing, branchy traversals, and more allocator pressure. Those are all costs we would rather avoid in this codebase.

Instead, the project uses an open-addressed table with Robin Hood insertion.

## The High-Level Shape

At a high level, the table is four parallel arrays:

- keys;
- values;
- cached full hashes;
- control bytes describing whether a slot is empty or full.

The set variant is the same table without the values array.

That means the set is not a separate algorithm. It is the same probing strategy with one column removed.

## Why Open Addressing Fits Arena Allocation

Arena allocators are a good fit when you want:

- a small number of large allocations;
- no per-object frees;
- simple ownership rules;
- bulk reset at a larger lifetime boundary.

Open addressing fits that model naturally.

When the table grows, it does not mutate individual entry ownership. It allocates a new, larger group of arrays from the arena and reinserts the live entries into that new storage. The old storage remains in the arena until the arena itself is reset or freed.

That gives us a simple rule:

- growth is allocate-and-rehash, never in-place node surgery.

## Power-of-Two Capacity and Bitmask Indexing

The table capacity is always normalized to a power of two.

That gives an efficient initial bucket computation:

```text
index = hash & (capacity - 1)
```

This is equivalent to modulo when capacity is a power of two, but it is cheaper and it simplifies wraparound arithmetic during probing.

The same trick is used to compute probe distance and to advance to the next slot.

## Why Robin Hood Hashing

This table uses Robin Hood hashing.

The central idea is simple:

- when two entries compete for a slot, the entry that is farther from its home position gets priority.

If the incoming entry has probed farther than the resident entry, the incoming one steals the slot and the resident entry continues probing.

That sounds aggressive, but it has a useful effect: it reduces variance in probe lengths.

Instead of letting some unlucky keys drift very far while other keys stay close to home, Robin Hood hashing tends to equalize displacement across entries. In practice that gives more stable lookup behavior than plain linear probing at similar load factors.

## Probe Distance

Every occupied entry has:

- a home slot determined by its hash;
- a current slot where it actually lives;
- a probe distance, which is how far it had to move from home.

With power-of-two capacity, wrapped distance is computed as:

```text
distance(slot, home) = (slot + capacity - home) & (capacity - 1)
```

That formula is cheap and handles wraparound correctly.

## Insertion Step by Step

The insertion algorithm used by the map version works like this:

1. Compute the incoming key's full hash.
2. Start at its home slot.
3. If the slot is empty, store key, value, and cached hash there.
4. If the slot contains the same key, overwrite the value.
5. Otherwise compare probe distances.
6. If the resident entry is closer to home than the incoming entry, swap them.
7. Continue probing with the displaced entry.

The set variant is the same algorithm, except there is no value to store or swap.

## Early Exit During Lookup

Robin Hood hashing gives a useful lookup optimization.

Suppose you are searching for a key at probe distance $d$. If you arrive at a resident entry whose probe distance is smaller than $d$, the searched key cannot appear later in the probe sequence.

Why?

Because if that key had existed, Robin Hood insertion would have forced it to steal from the smaller-distance resident earlier.

So lookup can stop early in two cases:

1. it hits an empty slot;
2. it reaches a resident entry whose displacement is smaller than the current search displacement.

That is one of the cleanest benefits of Robin Hood probing: insertion policy creates stronger lookup invariants.

## Why We Cache Full Hashes

The current design caches the full hash for every occupied slot.

That helps in three ways:

1. the resident home slot is derived from cached hash, not recomputed from key bytes;
2. equality checks can use hash equality as a cheap prefilter before key comparison;
3. rehash during growth can reuse the stored key with one normal insertion path.

This is not the most compressed metadata design possible, but it is a strong performance improvement for a small complexity increase.

## Why Separate Arrays

The table stores keys, values, hashes, and control bytes in separate arrays instead of one array of large slot structs.

There are trade-offs here.

Advantages:

- clear, regular layout;
- easy omission of values in the set specialization;
- cheap scans over control bytes or hashes in future optimizations;
- fewer mixed-field loads when only metadata is needed.

Costs:

- multiple parallel arrays must stay in lockstep;
- insertion and swap logic must update several arrays together.

For this project, the design is a good fit because it keeps the set specialization simple and leaves room for future metadata-focused optimizations.

## Why the Set Is `HashMap<K, void>`

Instead of writing a second independent hash-set implementation, the project uses a partial specialization:

- `HashMap<K, V>` stores keys and values;
- `HashMap<K, void>` stores only keys.

That means the set variant inherits:

- the same hashing traits;
- the same probe strategy;
- the same growth rules;
- the same error model.

The only major difference is that successful lookup returns a key pointer instead of a value pointer, and insertion does not carry or overwrite a mapped value.

## Load Factor Choice

The current implementation targets an 80% load factor.

That is a practical middle ground:

- lower load factors waste memory but reduce probe lengths;
- higher load factors improve memory efficiency but can lengthen probe chains.

Robin Hood hashing usually tolerates moderately high load factors better than naive linear probing, but there is still a real cost as occupancy rises. Around 80% is a common compromise for a general-purpose fast table that is not yet using more advanced metadata tricks.

## Growth by Rehash

When the next insertion would exceed the load threshold, the table grows.

The growth flow is:

1. choose a larger power-of-two capacity;
2. allocate fresh arrays from the arena;
3. initialize all control bytes to empty;
4. reinsert every live entry into the new storage;
5. publish the new arrays.

If allocation or reinsertion fails, the code restores the arena offset snapshot so a failed growth attempt does not silently leak arena space.

That rollback behavior matters because arena allocators are intentionally simple. If a failed reserve advanced the arena cursor permanently, even an error return would still damage future allocation capacity.

## Failure Model

The container reports failures through:

- `HashMapE`, the error enum;
- `HashMapR<T>`, the result alias.

That matches the broader project style:

- no exceptions;
- explicit success and failure states;
- lightweight error categories.

The two main operational failures today are:

- out-of-memory during reserve or growth;
- probe exhaustion, which should be exceptional if the table invariants hold.

## Usage Examples

The following examples show the intended calling style for the current API.

They are not trying to hide the low-level nature of the container. The design is explicit on purpose:

- callers provide an arena;
- callers handle `HashMapR<T>` directly;
- lookups return pointers rather than copies.

### Example: `HashMap<u64, u64>`

This is the simplest map case: integer keys and integer values.

```cpp
import core_types;
import core_memory;
import core_hash_map;

using namespace base;

void example_map() {
	Arena arena = {};
	bool ok = arena.init(MB(1));
	BASE_ASSERT(ok);

	HashMap<u64, u64> counts = {};

	auto init_r = counts.init(arena, 32);
	BASE_ASSERT(init_r.is_ok());

	auto ins_r = counts.upsert(arena, 42, 1);
	BASE_ASSERT(ins_r.is_ok());

	auto overwrite_r = counts.upsert(arena, 42, 99);
	BASE_ASSERT(overwrite_r.is_ok());
	BASE_ASSERT(overwrite_r.value == false);

	auto find_r = counts.find(42);
	BASE_ASSERT(find_r.is_ok());
	BASE_ASSERT(find_r.value.is_some());
	BASE_ASSERT(*find_r.value.unwrap() == 99);

	auto contains_r = counts.contains(7);
	BASE_ASSERT(contains_r.is_ok());
	BASE_ASSERT(contains_r.value == false);

	arena.free();
}
```

## What to Read Next

If you want to follow the full container story, read:

1. [Architecture Decisions](ARCHITECTURE_DECISIONS.md)
2. [Core Modules Guide](CORE_MODULES_GUIDE.md)
3. [core_hash_map module API](module__core__hash__map.html) after running `make docs`

# Future Enhancements Roadmap

This document tracks proposed improvements for the reusable core layer.

The focus is portability, tests, developer ergonomics, and the surrounding base-module surface.

## Planning Principles

- Keep the core library low-level, explicit, and allocation-aware.
- Preserve fast paths for systems-style projects that build on this layer.
- Add features behind narrow interfaces so downstream repos can opt in without carrying unrelated policy.
- Prefer incremental, testable milestones over large one-shot rewrites.

## 0. Hash Container Follow-Ups

### Goals

- Harden the arena-backed hash container.
- Preserve its current fast path while closing obvious functionality gaps.
- Keep map and set usage aligned under one implementation strategy.

### Proposed milestones

1. Container correctness coverage:
- Add focused regression tests for insert, overwrite, contains, and reserve-triggered rehash.
- Add coverage for both `HashMap<K, V>` and `HashMap<K, void>`.
- Exercise integer keys and `Str8` keys.

2. API completion:
- Decide whether to add `get_or_insert`, `find_or_default`, or similar convenience APIs.
- Evaluate whether `find(...) -> Option<pointer>` remains the primary lookup surface or whether a higher-level wrapper is needed.
- Clarify whether mutation through returned pointers is a supported public contract.

3. Erase strategy:
- Either add tombstone-based erase semantics to the current table,
- or explicitly keep this container insert/lookup-only and introduce deletion elsewhere.

4. Metadata optimization:
- Evaluate compact fingerprints in control bytes for faster negative lookups.
- Evaluate grouped probing or SIMD-assisted metadata scans if benchmarks justify the added complexity.

5. Arena lifecycle guidance:
- Document expected growth behavior when repeated rehashes occur in long-lived arenas.
- Decide whether a rebuild API should support caller-managed arena snapshots for bulk temporary maps.

### Success criteria

- Container behavior is documented and regression-tested.
- The public API is explicit about supported operations and failure modes.
- Performance changes are benchmarked rather than assumed.

## 1. Portability and Cross-Compiler Support

### Why this matters

The core layer is optimized for Linux plus GCC or Clang today, but it should remain a reusable foundation instead of a one-off project scaffold. The platform-sensitive pieces are the same ones most likely to need abstraction later:

- virtual memory calls (`mmap`, `munmap`, `madvise`, Linux flags);
- compiler-specific lifetime and attribute shims;
- formatting and type-width assumptions that vary between toolchains.

If the reusable layer should support Windows, broader Unix targets, or additional compilers, these areas need a clear backend boundary.

### Proposed milestones

1. Extend VM backends under core_vm:
- Reserve/commit/decommit/release (or map/unmap initially).
- Linux backend first (existing behavior).
- Windows backend next (`VirtualAlloc` and `VirtualFree`).

2. Make huge-page behavior optional and feature-gated:
- Use compile-time checks around Linux-specific flags.
- Keep huge-page hints best-effort and non-fatal.

Current direction:
- Huge-page hinting remains best-effort.
- Unsupported platforms should fail at compile time until an intentional backend is added.

3. Add compiler portability macros:
- Centralize always-inline and branch prediction attributes.
- Remove direct compiler attribute spelling from business logic.

4. Layer lifetime-start strategy:
- Use standard library lifetime-start API when available.
- Use compiler shim where required.
- Keep conservative fallback for trivially copyable types only.

5. Lock scalar ABI and vararg formatting policy:
- Add compile-time assertions that base aliases preserve required widths on all supported targets.
- Standardize C vararg formatting for fixed-width integers using inttypes macros instead of hard-coded length modifiers.
- Add portability tests that compile and execute representative formatting paths across supported toolchains.

### Success criteria

- Builds on Linux + GCC, Linux + Clang, and any additional supported backend.
- Core tests pass on all supported targets.
- No performance regression greater than 5% on benchmarked hot paths.

## 2. Core Tests Expansion

### Goals

- Increase confidence in the modules used by downstream projects.
- Catch UB-adjacent memory and parser regressions early.
- Provide deterministic behavior under debug and release builds.

### Proposed test categories

1. Arena and memory tests:
- Alignment guarantees for varied `alignof(T)` values.
- Capacity boundary and out-of-memory paths.
- Reset and scoped scratch rollback correctness.
- Lifetime-start path for trivially copyable types.

2. String and parser primitive tests:
- Slice bounds and offset invariants.
- Numeric parsing edge cases and overflow handling.
- Lexer coordinator diagnostic accumulation semantics.

3. Integration tests for core pipelines:
- File to token stream to parsed records.
- Mixed valid and invalid input recovery behavior.

### Success criteria

- Core modules have deterministic test coverage for critical error paths.
- Release-mode validation exists for the important boundaries.
- Regression tests exist for each fixed production bug.

## 3. Developer Ergonomics

### Goals

- Make the reusable layer easy to consume from other Orthodox C++ style projects.
- Keep developer workflows lightweight and explicit.
- Avoid spreading bespoke helper logic across downstream repositories.

### Proposed capabilities

- Shared command-line parsing helpers for small tools.
- Better generated docs entry points.
- A small reusable prelude surface that downstream repos can import directly.

### Suggested architecture

- Add focused modules only when a capability is used by more than one downstream consumer.
- Keep each module narrow and purpose-built.
- Prefer documentation that explains the layering and the intended import order.

### Success criteria

- A new consumer can understand the layer ordering quickly.
- Common utility code does not need to be copied into downstream repos.
- Documentation makes the intended public surface obvious.

## 4. Formatting API Strategy

### Goals

- Keep formatting allocation behavior explicit for arena-backed code.
- Provide a correctness-first path and a capped fast path.
- Avoid sentinel return values by preserving `Result<E, T>` semantics.

### Planned API split

1. Exact formatting path (two-pass):
- `base::str::pushf(...)` remains the exact-size formatter.
- Performs a sizing pass and then a write pass.
- Returns `Result<StringFormatError, Str8>`.
- Prioritizes correctness and precise allocation footprint.

2. Capped formatting path (single-pass):
- `base::str::push_cap(...)` is the capped formatter variant.
- Caller provides maximum output capacity.
- Performs one formatting pass into `cap + 1` bytes.
- Returns explicit truncation/failure via `StringFormatError`.
- Prioritizes throughput in hot paths with known practical bounds.

### Success criteria

- Both formatting modes are available with clear caller intent.
- No formatting API relies on ambiguous empty-string sentinels for failure.
- Arena offsets are restored on formatting failure or truncation paths.

## 5. Optional Input and Tooling Helpers

### Goals

- Support small reusable tooling in addition to puzzle-style executables.
- Keep network and file-format helpers separate from the base primitives.

### Proposed capabilities

- Chunked buffered file reader abstraction.
- Optional text streaming helpers for large inputs.
- Minimal HTTP utilities only if a second consumer actually needs them.

### Success criteria

- New helpers remain optional.
- The prelude stays small enough to understand quickly.
- Shared utilities are documented as either core primitives or opt-in extensions.

## Implementation Order Recommendation

1. Hash container follow-ups.
2. Core tests expansion.
3. Portability abstraction for memory and compiler attributes.
4. Formatting API strategy.
5. Developer ergonomics and shared tooling helpers.

## Open Questions

- Should additional backends be first-class now or deferred until needed?
- Which helpers belong in the reusable core layer versus a downstream app repo?
- Should shared tooling APIs favor callback pipelines, iterators, or both?

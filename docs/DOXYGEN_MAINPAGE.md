# Core Documentation Guide

Welcome to the authored documentation entry point for the `core` project.

This page is meant to keep the generated Doxygen output useful instead of turning it into a flat API dump. It gives you a guided path through the design notes, module map, and API reference.

## Recommended Reading Order

### 1. Architecture decisions

- [Architecture Decisions](ARCHITECTURE_DECISIONS.md)
- Durable design choices for arena allocation, portability boundaries, and container behavior.

### 2. HashMap algorithms guide

- [HashMap Algorithms](HASHMAP_ALGORITHMS.md)
- Tutorial-style walkthrough of Robin Hood probing, cached hashes, growth by rehash, and the `HashMap<K, void>` set specialization.

### 3. Future enhancements roadmap

- [Future Enhancements](FUTURE_ENHANCEMENTS.md)
- Forward-looking plan for portability, tests, ergonomics, and reusable tooling.

### 4. Core modules guide

- [Core Modules Guide](CORE_MODULES_GUIDE.md)
- High-level map of the foundational modules and how memory, strings, results, and hash containers fit together.

### 5. Contributor guidance

- [Contributing](CONTRIBUTING.md)
- Project rules, architectural boundaries, and documentation workflow.

### 6. Submodule consumption contract

- [Submodule Consumption Contract](SUBMODULE_CONTRACT.md)
- Official policy for release-only pinning, semver compatibility, CI drift checks, and downstream update workflow.

### 7. End-of-phase workflow

- [End-of-Phase Workflow](END_OF_PHASE_WORKFLOW.md)
- Required phase-closing process, including semver bumping and post-acceptance tag/release publication.

### 8. API reference

- Use the generated module, namespace, type, and file pages in the Doxygen sidebar and index pages.
- The generated module page for the aggregator is [core module](module__core.html).
- The generated module pages for foundational units are [core_types](module__core__types.html), [core_algorithm](module__core__algorithm.html), [core_memory](module__core__memory.html), [core_string](module__core__string.html), [core_hash](module__core__hash.html), and [core_hash_map](module__core__hash__map.html).
- The generated namespace pages for low-level APIs are under the `base` namespace tree.

## Documentation Layers

This project intentionally uses two complementary documentation layers.

### Authored guides

These are Markdown documents under `docs/`.

They answer questions like:

- Why was this design chosen?
- What tradeoffs matter here?
- What should future work improve?

### API reference

These are generated from Doxygen comments in source.

They answer questions like:

- What modules, namespaces, and types exist?
- What arguments and return values do they use?
- What invariants or preconditions apply?

## Current Highlights

- Arena allocation is explicit and low-level.
- Portability and VM boundaries are separated into dedicated core modules.
- Generic sorting helpers are centralized in `core_algorithm`.
- Strings, results, hashes, and containers are modeled as separate layers.
- The aggregator module re-exports the lower-level module set directly.

## Quick Links Into Rendered API

If you want to jump directly into generated reference material, these are the most useful entry points after running `make docs`:

- Core module page: `docs/html/module__core.html`
- Types module page: `docs/html/module__core__types.html`
- Algorithm module page: `docs/html/module__core__algorithm.html`
- Memory module page: `docs/html/module__core__memory.html`
- String module page: `docs/html/module__core__string.html`
- Hash module page: `docs/html/module__core__hash.html`
- HashMap module page: `docs/html/module__core__hash__map.html`
- Core modules guide: `docs/html/CORE_MODULES_GUIDE_8md.html`
- Architecture decisions: `docs/html/ARCHITECTURE__DECISIONS_8md.html`

## Navigating Core Material

If your focus is the base layer specifically, the fastest path is:

1. [Architecture Decisions](ARCHITECTURE_DECISIONS.md)
2. [HashMap Algorithms](HASHMAP_ALGORITHMS.md)
3. [Future Enhancements](FUTURE_ENHANCEMENTS.md)
4. [Core Modules Guide](CORE_MODULES_GUIDE.md)
5. [Contributing](CONTRIBUTING.md)
6. [Submodule Consumption Contract](SUBMODULE_CONTRACT.md)
7. [End-of-Phase Workflow](END_OF_PHASE_WORKFLOW.md)
8. [core module API](module__core.html)
9. [core_hash_map module API](module__core__hash__map.html)

That sequence gives you the design constraints first, then the module map, then the exact implementation surface.

## Rendered Docs Structure

The rendered Doxygen site uses a dedicated Markdown landing page so authored design notes and API reference live in one navigation flow.

- Landing page source: `docs/DOXYGEN_MAINPAGE.md`
- Rendered main page: `docs/html/index.html`
- Additional authored guides under `docs/` are included automatically by Doxygen.

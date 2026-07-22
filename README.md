# core

Reusable C++26 prelude and base-layer modules for low-level systems work.

This README is the documentation index and recommended reading path for the core library.

## Documentation Index

### Start here (core library users)

1. [docs/ARCHITECTURE_DECISIONS.md](docs/ARCHITECTURE_DECISIONS.md)
   - Durable design decisions for arena allocation, portability boundaries, and container behavior.

2. [docs/HASHMAP_ALGORITHMS.md](docs/HASHMAP_ALGORITHMS.md)
   - Standalone walkthrough of Robin Hood hashing, arena-backed growth, and the `HashMap<K, void>` set specialization.

3. [docs/FUTURE_ENHANCEMENTS.md](docs/FUTURE_ENHANCEMENTS.md)
   - Forward-looking roadmap for portability, tests, ergonomics, and reusable tooling.

4. [docs/CORE_MODULES_GUIDE.md](docs/CORE_MODULES_GUIDE.md)
   - Guided overview of the foundational modules and how memory, strings, results, and hash containers fit together.

5. [docs/CONTRIBUTING.md](docs/CONTRIBUTING.md)
   - Project rules, architectural boundaries, and documentation workflow guidance.

6. [docs/SUBMODULE_CONTRACT.md](docs/SUBMODULE_CONTRACT.md)
   - Official contract for downstream submodule pinning, semver compatibility expectations, CI drift checks, and update workflow.

7. [docs/END_OF_PHASE_WORKFLOW.md](docs/END_OF_PHASE_WORKFLOW.md)
   - Required phase-closing process, including semver bump selection and post-acceptance tag/release steps.

8. Doxygen API reference
   - Rendered landing page: [docs/html/index.html](docs/html/index.html)
   - Authored entry page: [docs/DOXYGEN_MAINPAGE.md](docs/DOXYGEN_MAINPAGE.md)
   - Configuration: [docs/Doxyfile](docs/Doxyfile)

## Rendered Docs Structure

The rendered Doxygen site uses a dedicated Markdown landing page so authored design notes and API reference live in one navigation flow.

- Landing page source: `docs/DOXYGEN_MAINPAGE.md`
- Rendered main page: `docs/html/index.html`
- Additional authored guides under `docs/` are included automatically by Doxygen.

## Versioning

- Current baseline: `v1.0.0`
- Semantic version tags should follow the `vMAJOR.MINOR.PATCH` pattern.
- Tagged releases are expected to mark the immutable baseline for downstream consumers.

## Notes

- The default build configuration is debug.
- The compilation database is generated metadata and should be refreshed when compiler flags or source layout change.
- The documentation set is meant to explain the reusable layer first and the implementation details second.

# Contributing to core

This document captures the project rules and workflow guidance for the reusable core layer.

## Scope note

The core repository is a low-level foundation for Orthodox C++ style projects. Contributions should improve the reusable prelude/base layer rather than introducing app-specific policy.

## Design Priorities

- Keep code explicit and allocation-aware.
- Prefer small, composable modules.
- Preserve `-fno-exceptions` and `-fno-rtti` compatibility.
- Avoid hidden ownership, implicit lifetimes, and opaque runtime dependencies.
- Favor Linux-first implementations when the abstraction clearly belongs in the platform layer.

## Module Boundaries

- `core_types` owns scalar aliases and tiny arithmetic helpers.
- `core_algorithm` owns reusable sorting and comparator primitives.
- `core_portability` owns compiler and standard-library shims.
- `core_vm` owns operating-system virtual memory behavior.
- `core_memory` owns arena policy and allocation mechanics.
- `core_string`, `core_result`, `core_hash`, `core_hash_map`, `core_text_parse`, `core_lexer`, `core_lexer_coord`, `core_console`, and `core_file` should stay focused on their narrow responsibilities.

If a feature does not fit one of those layers cleanly, it probably needs a new module or a narrower API surface.

## Documentation Expectations

- Update authored docs when behavior or contracts change.
- Keep module guides, roadmaps, and architecture notes aligned with the source.
- Prefer Doxygen comments for API-level facts and Markdown for design rationale.
- If a public module name, namespace, or import path changes, update the generated and authored docs together.
- Keep downstream consumption policy aligned with [SUBMODULE_CONTRACT.md](SUBMODULE_CONTRACT.md).
- Keep the reusable CI check template aligned with `docs/templates/verify_submodule_pin.sh`.

## Change Style

- Prefer the smallest logical edit that fixes the problem at the right layer.
- Do not add complexity just to mirror a downstream repo.
- When a behavior change affects downstream usage, add or update a focused test.
- If a change exposes a missing guide or roadmap item, add the documentation in the same pass.

## Suggested Workflow

1. Read the nearest module guide or architecture note.
2. Make the smallest correct change in the controlling module.
3. Rebuild the relevant mode.
4. Regenerate docs if the change affects public surface or documentation.
5. Verify the result with a focused test or build command.

## End-of-Phase Rule

- The phase-closing workflow is mandatory and is defined in [END_OF_PHASE_WORKFLOW.md](END_OF_PHASE_WORKFLOW.md).
- In `cpp-core`, this includes semver bump selection plus a new tag and GitHub Release after acceptance.

## README and Docs Index

The repository README is the main index for contributors and readers.
The authored docs under `docs/` provide the design rationale and API entry points.

## Example Contribution Checklist

Before opening a reviewable change, check that:

- the code still builds in the intended mode;
- the docs reflect the new behavior or contract;
- any public surface change is discoverable from the README or docs landing page;
- the change does not broaden the module boundary unnecessarily.

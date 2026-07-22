# Submodule Consumption Contract

This document defines the official contract for consuming `cpp-core` as a git submodule in downstream repositories.

The goal is to keep upgrades predictable, tie compatibility to semantic versioning, and avoid silent submodule drift.

## 1. Release-Only Upgrade Rule

Downstream repositories should upgrade the `cpp-core` submodule only to released tags such as `v1.0.0`.

Do not point consumers at random, untagged commits.

Why:

- tags are reviewable, documented, and immutable reference points;
- release notes capture behavior changes and migration context;
- rollbacks are straightforward and auditable.

## 2. SemVer Compatibility Contract

`cpp-core` uses semantic versioning with tags in the form `vMAJOR.MINOR.PATCH`.

Compatibility expectations:

- `PATCH` version increments: backward-compatible bug fixes and documentation clarifications.
- `MINOR` version increments: backward-compatible feature additions and non-breaking API growth.
- `MAJOR` version increments: breaking API, behavior, module-surface, or integration changes.

Downstream policy:

- safe auto-adoption scope: `PATCH` updates within the pinned `MAJOR.MINOR` line;
- review-required scope: all `MINOR` and `MAJOR` updates.

## 3. CI Drift Guard (Required)

Each downstream repository should include a CI check that verifies the submodule is pinned to a released tag and not to an arbitrary commit.

Suggested check policy:

- fail if submodule HEAD has no exact matching tag;
- fail if the tag does not match `vMAJOR.MINOR.PATCH`;
- optionally enforce an allow-list or minimum version.

Template shell check (source of truth in this repo):

- `docs/templates/verify_submodule_pin.sh`

Example shell check for downstream CI:

```sh
#!/usr/bin/env sh
set -eu

submodule_path="third_party/cpp-core"

if ! git -C "$submodule_path" rev-parse --verify HEAD >/dev/null 2>&1; then
  echo "cpp-core submodule is missing or not initialized"
  exit 1
fi

tag="$(git -C "$submodule_path" tag --points-at HEAD | head -n 1 || true)"

if [ -z "$tag" ]; then
  echo "cpp-core submodule is not pinned to a release tag"
  exit 1
fi

case "$tag" in
  v[0-9]*.[0-9]*.[0-9]*)
    echo "cpp-core pinned to release tag: $tag"
    ;;
  *)
    echo "cpp-core tag is not semver-shaped: $tag"
    exit 1
    ;;
esac
```

### How downstream repos should use the template

1. Copy the template into the downstream repo, for example:

```sh
mkdir -p ci
cp docs/templates/verify_submodule_pin.sh ../your-downstream-repo/ci/verify_cpp_core_submodule.sh
```

2. Run it in CI after submodule checkout/update.

3. Set path and optional tag policy via environment variables when needed:

```sh
CPP_CORE_SUBMODULE_PATH=third_party/cpp-core \
CPP_CORE_TAG_PATTERN='v[0-9]*.[0-9]*.[0-9]*' \
sh ci/verify_cpp_core_submodule.sh
```

### GitHub Actions example

```yaml
name: Verify cpp-core submodule pin

on: [push, pull_request]

jobs:
  verify-cpp-core-pin:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
        with:
          submodules: recursive

      - name: Verify cpp-core tag pin
        env:
          CPP_CORE_SUBMODULE_PATH: third_party/cpp-core
        run: sh ci/verify_cpp_core_submodule.sh
```

## 4. Standard Downstream Update Workflow

Use this same workflow in every consumer repository.

1. Fetch submodule tags:

```sh
git -C third_party/cpp-core fetch --tags origin
```

2. Checkout the target release tag:

```sh
git -C third_party/cpp-core checkout vX.Y.Z
```

3. Record the submodule pointer update:

```sh
git add third_party/cpp-core
```

4. Commit with a consistent message:

```sh
git commit -m "chore(submodule): bump cpp-core to vX.Y.Z"
```

5. Run integration build and tests in the downstream repository.

6. Include release notes summary in the PR body when relevant.

## 5. Rollback Workflow

If a downstream upgrade causes regressions:

1. Checkout the previous known-good tag in the submodule.
2. Commit the pointer rollback.
3. Open a follow-up issue against `cpp-core` with repro details.

## 6. Ownership and Change Control

- `cpp-core` maintainers own semver decisions and release cadence.
- Downstream maintainers own upgrade timing and integration validation.
- Breaking changes must be called out in release notes and require a MAJOR bump.

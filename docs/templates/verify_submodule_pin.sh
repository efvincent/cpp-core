#!/usr/bin/env sh
set -eu

# Verifies that a submodule is pinned to an exact semver-style release tag.
#
# Optional environment variables:
# - CPP_CORE_SUBMODULE_PATH: submodule path in the consumer repo
#   default: third_party/cpp-core
# - CPP_CORE_TAG_PATTERN: shell-case pattern for accepted tags
#   default: v[0-9]*.[0-9]*.[0-9]*

submodule_path="${CPP_CORE_SUBMODULE_PATH:-third_party/cpp-core}"
tag_pattern="${CPP_CORE_TAG_PATTERN:-v[0-9]*.[0-9]*.[0-9]*}"

if ! git -C "$submodule_path" rev-parse --verify HEAD >/dev/null 2>&1; then
  echo "cpp-core submodule is missing or not initialized at: $submodule_path"
  exit 1
fi

# Prefer exact tag for HEAD; if multiple tags point to the same commit, pick first.
tag="$(git -C "$submodule_path" tag --points-at HEAD | head -n 1 || true)"

if [ -z "$tag" ]; then
  echo "cpp-core submodule is not pinned to a release tag"
  exit 1
fi

case "$tag" in
  $tag_pattern)
    echo "cpp-core pinned to release tag: $tag"
    ;;
  *)
    echo "cpp-core tag does not match required pattern: $tag (expected: $tag_pattern)"
    exit 1
    ;;
esac

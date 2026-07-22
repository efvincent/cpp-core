# End-of-Phase Workflow

This document defines the required end-of-phase workflow for `cpp-core`.

It follows the same baseline process used in `aoc-cpp`, with one additional requirement for `cpp-core`: semantic version bumping and a new tag/release after acceptance.

## Scope

Use this workflow whenever a phase of planned work is complete and ready to be shipped for downstream consumption.

## 1. Branch Gate

If the current branch is `main`, do not ship directly from it.

Create and use a dedicated working branch first.

## 2. Pre-PR Validation

Before opening or updating a PR:

1. Run a code review pass with focus on behavior regressions, correctness risks, and missing tests.
2. Audit authored docs and relevant Doxygen comments for accuracy.
3. Apply any required fixups so code and docs match the intended contract.
4. Regenerate docs if the public surface or documentation changed.

## 3. PR Preparation

1. Push the phase branch.
2. Open or update a PR with a detailed body, including:
- what changed;
- why it changed;
- compatibility notes for downstream submodule consumers;
- any migration or rollout notes.

## 4. Acceptance Gate

Do not create a release tag before the phase PR is accepted.

Acceptance means the phase changes are approved and merged.

## 5. SemVer Decision (Required)

After acceptance, determine the next version according to semantic versioning:

- `PATCH`: backward-compatible bug fixes or documentation clarifications.
- `MINOR`: backward-compatible feature additions.
- `MAJOR`: breaking API, behavior, module-surface, or integration changes.

## 6. Version Bump Artifacts

For the chosen version `vX.Y.Z`:

1. Update the root `VERSION` file to `X.Y.Z`.
2. Update `PROJECT_NUMBER` in `docs/Doxyfile` to `X.Y.Z`.
3. Update release-facing documentation if needed (for example README version section).
4. Commit the version-bump change.

## 7. Tag and Release (After Acceptance)

Create and publish the release only after the accepted version-bump commit is on `main`.

1. Create an annotated tag:

```sh
git tag -a vX.Y.Z -m "core vX.Y.Z"
```

2. Push the tag:

```sh
git push origin vX.Y.Z
```

3. Create a GitHub Release for `vX.Y.Z` with clear notes:
- highlights;
- compatibility impact;
- downstream submodule guidance.

### Release command block (copy/paste)

Use this block after the accepted version-bump commit is on `main`:

```sh
VERSION="X.Y.Z"
TAG="v${VERSION}"

# 1) Create and push annotated tag
git tag -a "$TAG" -m "core $TAG"
git push origin "$TAG"

# 2) Prepare release notes in a file (avoids shell quoting issues)
cat > /tmp/cpp-core-${TAG}-notes.md <<EOF
Release ${TAG}

Highlights:
- <summary of key changes>

Compatibility:
- SemVer impact: <PATCH|MINOR|MAJOR>
- Downstream action: update submodule pin to ${TAG}

References:
- Submodule contract: docs/SUBMODULE_CONTRACT.md
EOF

# 3) Publish GitHub Release
gh release create "$TAG" \
	--title "cpp-core ${TAG}" \
	--notes-file "/tmp/cpp-core-${TAG}-notes.md"
```

## 8. Downstream Communication

For downstream repositories using `cpp-core` as a submodule:

1. Reference the new release tag.
2. Call out whether the release is patch/minor/major.
3. Link migration notes when relevant.
4. Follow the standard downstream update process in [SUBMODULE_CONTRACT.md](SUBMODULE_CONTRACT.md).

## 9. Release Checklist

Before closing the phase, confirm all of the following:

- phase PR accepted and merged;
- semver bump chosen and justified;
- `VERSION` updated;
- `docs/Doxyfile` `PROJECT_NUMBER` updated;
- release tag created and pushed;
- GitHub Release published;
- downstream update guidance communicated.

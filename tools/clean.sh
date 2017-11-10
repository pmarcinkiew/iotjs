#!/bin/bash

set -e
set -u

DEBUG=y
DRY_RUN=

function die() {
  echo "$@" >&2
  exit 1
}

function execute() {
  [ -z "${DEBUG}" ] || echo "@$PWD: $@"
  [ -n "${DRY_RUN}" ] || "$@"
}

function gently_clean() {
  execute git stash save --include-untracked
  execute git clean -dfx
  execute [ -z "$(git stash list)" ] || git stash apply
}

function save_and_clean() {
  set +e
  execute git commit -am "Autosave commit"
  execute git checkout -b "autosave"
  set -e
  execute git reset --hard
  execute git clean -dfx
}

git log 8a33a8172254c60ee8fa -1 >/dev/null || die "Not the iotjs git repository?"

# Reset iotjs repository
echo Resetting "iotjs"
execute rm -rf build/
gently_clean
# Not doing git clean due to submodules

# Save'n'clean submodules' states
for sub in http-parser jerry libtuv tizenrt; do
  echo Resetting "deps/${sub}"
  pushd "deps/${sub}" >/dev/null
  if [ "${sub}" == "tizenrt" ]; then
    save_and_clean
  else
    gently_clean
  fi
  popd >/dev/null
done

# Reset submodules
execute git submodule sync
execute git submodule update

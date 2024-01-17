#!/bin/sh

# Copyright (c) 2024 Rivos, Inc.
# SPDX-License-Identifier: Apache2

EXPECTED_VERSION="16"

# find clang-format executable: either 'clang-format-16' or 'clang-format'
for ver_suffix in "-${EXPECTED_VERSION}" ""; do
    clangformat="$(which clang-format${ver_suffix} 2>/dev/null)"
    if [ -n "${clangformat}" ]; then
        break
    fi
done
if [ -z "${clangformat}" ]; then
    echo "Unable to locate clang-format" >&2
    exit 1
fi

# check clang-format version
version_full="$(${clangformat} --version | head -1 | \
    sed -E 's/^.*clang-format version ([0-9]+\.[0-9]+\.[0-9]+).*$/\1/')"
version_major="$(echo ${version_full} | cut -d. -f1)"
if [ ${version_major} -lt ${EXPECTED_VERSION} ]; then
    echo "clang-format v${EXPECTED_VERSION} required, found ${version_full}" >&2
    exit 1
else
    if [ ${version_major} -ne ${EXPECTED_VERSION} ]; then
        echo -n "clang-format v${EXPECTED_VERSION} expected, " >&2
        echo "v${version_full} may generate unexpected results" >&2
    fi
fi

# build config path
CFG="$(dirname $0)"/clang-format.yml

exec "${clangformat}" --style=file:"${CFG}" $*

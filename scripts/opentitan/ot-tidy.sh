#!/bin/sh

# Copyright (c) 2024 Rivos, Inc.
# SPDX-License-Identifier: Apache2

EXPECTED_VERSION="16"

# find clang-tidy executable: either 'clang-tidy-16' or 'clang-tidy'
for ver_suffix in "-${EXPECTED_VERSION}" ""; do
    clangtidy="$(which clang-tidy${ver_suffix} 2>/dev/null)"
    if [ -n "${clangtidy}" ]; then
        break
    fi
done
if [ -z "${clangtidy}" ]; then
    echo "Unable to locate clang-tidy" >&2
    exit 1
fi

# check clang-tidy version
version_full="$(${clangtidy} --version | head -1 | \
    sed -E 's/^.*LLVM version ([0-9]+\.[0-9]+\.[0-9]+).*$/\1/')"
version_major="$(echo ${version_full} | cut -d. -f1)"
if [ ${version_major} -lt ${EXPECTED_VERSION} ]; then
    echo "clang-tidy v${EXPECTED_VERSION} required, found ${version_full}" >&2
    exit 1
else
    if [ ${version_major} -ne ${EXPECTED_VERSION} ]; then
        echo -n "clang-tidy v${EXPECTED_VERSION} expected, " >&2
        echo "v${version_full} may generate unexpected results" >&2
    fi
fi

# build config path
CFG="$(dirname $0)"/clang-tidy.yml

exec "${clangtidy}" --config-file="${CFG}" $*

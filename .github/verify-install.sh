#!/usr/bin/env bash
# Installs the built effect into a staging directory and checks that the plugins
# land where KWin looks for them, under the exact names KWin derives ids from.
#
# KPluginMetaData takes the plugin id from the file name, and that id is what
# KWin writes to kwinrc and what the config module passes to reconfigureEffect().
# A stray "lib" prefix therefore silently renames the effect, so assert on it.
set -euo pipefail

stage="${PWD}/stage"
rm -rf "${stage}"
DESTDIR="${stage}" cmake --install build

echo "Installed files:"
installed="$(cd "${stage}" && find . -name '*.so' | sort)"
echo "${installed}"

fail=0
expect() {
    if ! grep -q "$1\$" <<<"${installed}"; then
        echo "::error::expected an installed plugin matching '$1'"
        fail=1
    fi
}

expect 'kwin/effects/plugins/kwin4_effect_yetanothermagiclamp.so'
expect 'kwin/effects/configs/kwin_yetanothermagiclamp_config.so'

# The effect is useless to KWin without its embedded metadata: PluginEffectLoader
# skips plugins whose KPluginMetaData is invalid, so the effect would simply not
# show up in System Settings. Check for a string only metadata.json carries.
effect="$(cd "${stage}" && find . -path '*kwin/effects/plugins/*.so' | head -1)"
if [[ -n "${effect}" ]] && ! grep -aq 'exclusiveGroup' "${stage}/${effect#./}"; then
    echo "::error::metadata.json was not embedded into ${effect}"
    fail=1
fi

exit "${fail}"

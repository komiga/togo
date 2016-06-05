#!/bin/bash

pushd "$(dirname "$(readlink -f "$0")")/../dep/glad" > /dev/null

python -m glad \
	--local-files \
	--omit-khrplatform \
	--generator c \
	--profile core \
	--spec gl \
	--api gl=3.3 \
	--extensions "../../scripts/glad-extensions.txt" \
	--out-path "./src/glad/"

popd > /dev/null

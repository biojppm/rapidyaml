#!/bin/bash -x

gh run list --json databaseId -q '.[].databaseId' | xargs -d '\n' -n1 gh run cancel

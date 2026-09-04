#!/usr/bin/env bash
# Nautilus Playground redeploy: fetches origin/main, and — only if it moved —
# rebuilds the runner + API images and swaps the running container over.
#
# Runs on the host that owns the docker daemon serving the playground
# (currently the nautilus-playground LXC), against a real git clone at
# $REPO_DIR (not the plain source copy earlier deploys used). Intended to be
# driven by cron every few hours:
#   0 */4 * * * /opt/nautilus-playground/src/tools/playground/deploy/redeploy.sh >> /opt/nautilus-playground/deploy.log 2>&1
set -euo pipefail

REPO_DIR="${REPO_DIR:-/opt/nautilus-playground/src}"
REMOTE="${REMOTE:-origin}"
BRANCH="${BRANCH:-main}"
LOCK_FILE="${LOCK_FILE:-/var/tmp/nautilus-playground-deploy.lock}"

exec 9>"$LOCK_FILE"
if ! flock -n 9; then
	echo "$(date -Is) redeploy already in progress, skipping"
	exit 0
fi

cd "$REPO_DIR"
git fetch "$REMOTE" "$BRANCH"
local_sha="$(git rev-parse HEAD)"
remote_sha="$(git rev-parse "$REMOTE/$BRANCH")"

if [ "$local_sha" = "$remote_sha" ]; then
	echo "$(date -Is) up to date at ${local_sha:0:8}, nothing to do"
	exit 0
fi

echo "$(date -Is) deploying ${remote_sha:0:8} (was ${local_sha:0:8})"
git reset --hard "$REMOTE/$BRANCH"

# Every build here is a full nautilus+MLIR rebuild (the runner Dockerfile
# COPies the whole tree before the cmake layer, so any source change busts
# that cache) and image tags pile up one-per-deploy — prune first or the
# disk fills up within a few redeploys.
docker image prune -a -f
docker builder prune -a -f

short_sha="$(git rev-parse --short HEAD)"
docker build -f tools/playground/runner/Dockerfile.runner \
	-t "nautilus-playground-runner:${short_sha}" -t nautilus-playground-runner:latest .
docker build -f tools/playground/server/Dockerfile \
	--build-arg GIT_COMMIT="${short_sha}" \
	-t "nautilus-playground-api:${short_sha}" -t nautilus-playground-api:latest .

docker rm -f nautilus-playground >/dev/null 2>&1 || true
docker run -d --name nautilus-playground --init --restart unless-stopped \
	-p 8080:8080 \
	-e RUNNER_IMAGE=nautilus-playground-runner \
	-e TRUST_PROXY=1 \
	-e JOB_ROOT=/var/tmp/nautilus-playground-jobs \
	-v /var/run/docker.sock:/var/run/docker.sock \
	-v /var/tmp/nautilus-playground-jobs:/var/tmp/nautilus-playground-jobs \
	nautilus-playground-api:latest

echo "$(date -Is) deployed ${short_sha}"

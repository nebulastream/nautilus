# Playground Auto-Deploy Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Deploy a health-checked Docker rebuild of Nautilus `main` to the `nautilus-playground` LXC no more than four hours after an upstream change.

**Architecture:** A root-owned script inside Proxmox LXC 101 serializes runs with `flock`, fetches the public Nautilus checkout, and exits without building when `origin/main` has not changed. When it has, it builds commit-tagged runner and API images, verifies an isolated API candidate, promotes it to port 8080, and restores the previous container if the production check fails. A systemd timer invokes the service every four hours with a small jitter and records output in journald.

**Tech Stack:** Debian systemd, Git, Docker/BuildKit, Bash, curl.

## Global Constraints

- Install only inside Proxmox LXC 101 (`nautilus-playground`); do not alter the Proxmox host or the local developer checkout.
- Deploy only `https://github.com/nebulastream/nautilus.git` branch `main`.
- Preserve the existing production container environment, Docker socket mount, job-root mount, port 8080 binding, and `unless-stopped` restart policy.
- Build both `nautilus-playground-runner` and `nautilus-playground-api` from the same full commit SHA.
- Do not interrupt the running production container until an isolated candidate has answered `GET /api/meta` successfully.
- On a failed production health check, restore the previous container automatically.
- Run at most one deployment at once; retain only the current production container and commit-tagged current images after success.

---

### Task 1: Install the revision-aware deployment script

**Files:**
- Create: `/usr/local/sbin/nautilus-playground-deploy`
- Create: `/etc/nautilus-playground/deployed-revision`
- Test: `/tmp/nautilus-playground-deploy-test.log`

**Interfaces:**
- Consumes: `GIT_URL=https://github.com/nebulastream/nautilus.git`, `/opt/nautilus-playground-src`, Docker image tags `nautilus-playground-runner:<sha>` and `nautilus-playground-api:<sha>`.
- Produces: a zero exit without rebuilding when `/etc/nautilus-playground/deployed-revision` equals `origin/main`; otherwise it writes the successfully deployed full SHA to that file.

- [ ] **Step 1: Install the script with the deployment contract**

Create `/usr/local/sbin/nautilus-playground-deploy` with mode `0750`. It must:

```bash
#!/usr/bin/env bash
set -Eeuo pipefail

readonly GIT_URL='https://github.com/nebulastream/nautilus.git'
readonly SOURCE_DIR='/opt/nautilus-playground-src'
readonly STATE_DIR='/etc/nautilus-playground'
readonly REVISION_FILE="$STATE_DIR/deployed-revision"
readonly LOCK_FILE='/run/lock/nautilus-playground-deploy.lock'

exec 9>"$LOCK_FILE"
flock -n 9 || { echo 'another deployment is already running'; exit 0; }
mkdir -p "$STATE_DIR"

if [[ ! -d "$SOURCE_DIR/.git" ]]; then
	git clone --depth 1 --branch main "$GIT_URL" "$SOURCE_DIR"
fi
git -C "$SOURCE_DIR" fetch --depth 1 origin main
readonly REVISION="$(git -C "$SOURCE_DIR" rev-parse origin/main)"
if [[ -f "$REVISION_FILE" && "$(<"$REVISION_FILE")" == "$REVISION" ]]; then
	echo "already deployed: $REVISION"
	exit 0
fi
git -C "$SOURCE_DIR" checkout --detach --force "$REVISION"
```

- [ ] **Step 2: Add image building and candidate validation**

Append the following script functions. The candidate uses a dynamically allocated loopback port and is always removed through the cleanup trap.

```bash
readonly RUNNER_IMAGE="nautilus-playground-runner:$REVISION"
readonly API_IMAGE="nautilus-playground-api:$REVISION"
readonly CANDIDATE='nautilus-playground-candidate'

cleanup_candidate() {
	docker rm -f "$CANDIDATE" >/dev/null 2>&1 || true
}
trap cleanup_candidate EXIT

docker build -f "$SOURCE_DIR/tools/playground/runner/Dockerfile.runner" -t "$RUNNER_IMAGE" "$SOURCE_DIR"
docker build -f "$SOURCE_DIR/tools/playground/server/Dockerfile" -t "$API_IMAGE" "$SOURCE_DIR"

for port in $(seq 18081 18120); do
	if ! ss -ltn "sport = :$port" | grep -q LISTEN; then
		readonly CANDIDATE_PORT="$port"
		break
	fi
done
: "${CANDIDATE_PORT:?no free candidate port}"

docker run -d --name "$CANDIDATE" --restart no -p "127.0.0.1:$CANDIDATE_PORT:8080" \
	-e "RUNNER_IMAGE=$RUNNER_IMAGE" -e TRUST_PROXY=1 -e JOB_ROOT=/var/tmp/nautilus-playground-jobs \
	-v /var/run/docker.sock:/var/run/docker.sock \
	-v /var/tmp/nautilus-playground-jobs:/var/tmp/nautilus-playground-jobs \
	"$API_IMAGE"

for attempt in $(seq 1 30); do
	if curl --fail --silent --show-error "http://127.0.0.1:$CANDIDATE_PORT/api/meta" >/dev/null; then
		break
	fi
	sleep 1
done
curl --fail --silent --show-error "http://127.0.0.1:$CANDIDATE_PORT/api/meta" >/dev/null
```

- [ ] **Step 3: Add promotion, rollback, and revision recording**

Append this block. It holds the prior container under `nautilus-playground-backup` until the new production binding passes its own health check.

```bash
docker stop nautilus-playground
docker rename nautilus-playground nautilus-playground-backup

if ! docker run -d --name nautilus-playground --restart unless-stopped -p 8080:8080 \
	-e "RUNNER_IMAGE=$RUNNER_IMAGE" -e TRUST_PROXY=1 -e JOB_ROOT=/var/tmp/nautilus-playground-jobs \
	-v /var/run/docker.sock:/var/run/docker.sock \
	-v /var/tmp/nautilus-playground-jobs:/var/tmp/nautilus-playground-jobs \
	"$API_IMAGE"; then
	docker rename nautilus-playground-backup nautilus-playground
	docker start nautilus-playground
	exit 1
fi

if ! curl --fail --silent --show-error --retry 30 --retry-connrefused --retry-delay 1 \
	"http://127.0.0.1:8080/api/meta" >/dev/null; then
	docker rm -f nautilus-playground
	docker rename nautilus-playground-backup nautilus-playground
	docker start nautilus-playground
	exit 1
fi

docker rm nautilus-playground-backup
printf '%s\n' "$REVISION" >"$REVISION_FILE"
docker image tag "$RUNNER_IMAGE" nautilus-playground-runner:latest
docker image tag "$API_IMAGE" nautilus-playground-api:latest
echo "deployed $REVISION"
```

- [ ] **Step 4: Test the no-change path**

Run:

```bash
systemd-run --wait --pipe /usr/local/sbin/nautilus-playground-deploy
```

Expected: exit code `0` and `already deployed: 69cac8bc373b53777f0dbe2491425ed751e17762`; no Docker container is recreated.

### Task 2: Register the four-hour systemd schedule

**Files:**
- Create: `/etc/systemd/system/nautilus-playground-deploy.service`
- Create: `/etc/systemd/system/nautilus-playground-deploy.timer`
- Test: `systemctl list-timers nautilus-playground-deploy.timer`

**Interfaces:**
- Consumes: executable `/usr/local/sbin/nautilus-playground-deploy` from Task 1.
- Produces: timer `nautilus-playground-deploy.timer`, active at boot and every four hours after the previous successful activation.

- [ ] **Step 1: Create the oneshot service**

Create `/etc/systemd/system/nautilus-playground-deploy.service`:

```ini
[Unit]
Description=Deploy Nautilus Playground from upstream main
Wants=network-online.target
After=network-online.target docker.service
Requires=docker.service

[Service]
Type=oneshot
ExecStart=/usr/local/sbin/nautilus-playground-deploy
```

- [ ] **Step 2: Create the persistent four-hour timer**

Create `/etc/systemd/system/nautilus-playground-deploy.timer`:

```ini
[Unit]
Description=Check Nautilus Playground upstream main every four hours

[Timer]
OnBootSec=15min
OnUnitActiveSec=4h
RandomizedDelaySec=10min
Persistent=true
Unit=nautilus-playground-deploy.service

[Install]
WantedBy=timers.target
```

- [ ] **Step 3: Enable and verify the timer**

Run:

```bash
systemctl daemon-reload
systemctl enable --now nautilus-playground-deploy.timer
systemctl is-enabled nautilus-playground-deploy.timer
systemctl is-active nautilus-playground-deploy.timer
systemctl list-timers nautilus-playground-deploy.timer --all
```

Expected: `enabled`, `active`, and a next activation approximately four hours after the service’s most recent activation, plus up to ten minutes of jitter.

### Task 3: Verify operational behavior

**Files:**
- Test: `journalctl -u nautilus-playground-deploy.service`
- Test: `docker ps --filter name=nautilus-playground`

**Interfaces:**
- Consumes: service and timer from Task 2.
- Produces: a working service on port 8080 and journal evidence of the no-change deployment decision.

- [ ] **Step 1: Run the service once manually**

Run:

```bash
systemctl start nautilus-playground-deploy.service
journalctl -u nautilus-playground-deploy.service -n 30 --no-pager
```

Expected: the journal reports either `already deployed: <sha>` or `deployed <sha>` and systemd reports success.

- [ ] **Step 2: Verify the public runtime contract**

Run:

```bash
docker ps --filter name=nautilus-playground --format '{{.Names}} {{.Image}} {{.Status}} {{.Ports}}'
curl --fail --silent --show-error http://127.0.0.1:8080/api/meta
```

Expected: exactly one `nautilus-playground` container is up with `0.0.0.0:8080->8080/tcp`, and the API metadata JSON includes `enableDwarf`.

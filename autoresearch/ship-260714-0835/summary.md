# Ship Summary — Nautilus Playground on Proxmox

**Live at: http://192.168.178.32:8080** (web UI + API on the same port; Caddy/TLS layer skipped by request — plain IP:port).

## What was deployed
- `nautilus-playground-runner` (1.45 GB) and `nautilus-playground-api` (497 MB) images, built on the host from a `git archive` of `fc113f43`.
- API container: `docker run -d --init --name nautilus-playground --restart unless-stopped -p 8080:8080 -e RUNNER_IMAGE=nautilus-playground-runner -e JOB_ROOT=/var/tmp/nautilus-playground-jobs -v /var/run/docker.sock:/var/run/docker.sock -v /var/tmp/nautilus-playground-jobs:/var/tmp/nautilus-playground-jobs nautilus-playground-api`
- Source staged at `/opt/nautilus-playground/src` on the host (with `-j4` build cap and the dockerRunner fix applied).

## Host changes
1. Installed `docker.io` 26.1.5 on the Proxmox node (enabled + started).
2. Installed `/etc/apparmor.d/docker-default` — upstream docker-default profile plus `abi <abi/4.0>,` and `unix,`. Debian 13's AppArmor 4.1 mediates unix sockets; docker.io 26.x's built-in profile predates that and containers got EACCES creating unix stream socketpairs (broke npm in image builds). Boot-persistent; survives dockerd restarts.

## Code fixes (in local working tree, NOT committed)
- `tools/playground/server/src/sandbox/dockerRunner.ts` — `chmod(outDir, 0o777)` after mkdir: stock Linux daemons preserve host ownership on bind mounts, so the runner's non-root user couldn't write `/out` (jobs failed in ~450 ms with empty stderr). Docker Desktop masked this.
- `tools/playground/runner/smoke/run_smoke.sh` — same `chmod 777` for smoke job dirs.

## Verification
- `GET /` 200, asset bundle 200, `GET /api/meta` lists all 7 backends.
- End-to-end compile of example `01_add_mul` via `POST /api/compile` from another machine: status `done`, 8 stages (trace → SSA → IR → CFG → optimized IR → MLIR → LLVM IR ×2), sandbox 1.17 s.
- Full smoke suite on host: all fixtures × all backends (mlir, cpp, bc, tbc, asmjit, cuda, metal) + negative fixtures — **all passed**.

## Notes
- Per-IP rate limit (10/min) is active against direct client IPs (`TRUST_PROXY` unset — correct without a proxy).
- GPU backends run in codegen-only mode (no GPU on the host) — by design.
- Host RAM headroom is thin (~5.5 GB free with VMs running); job concurrency ≈ 2 × 2 GB caps fits.

## Update (same day): migrated into LXC 101
Per follow-up request the deployment moved into a proper Proxmox guest: unprivileged Debian 13 LXC 101 "nautilus-playground" (`nesting=1,keyctl=1`, 8 cores / 6 GB RAM / 24 GB local-lvm, onboot=1). Images moved via docker save → pct push → docker load; same run command inside. **New URL: http://192.168.178.40:8080** (DHCP). Verified: E2E compile from LAN (done, 8 stages, 1.21 s) and full smoke suite inside the LXC — all passed. The AppArmor docker-default issue does not occur inside the unprivileged LXC (nested Docker runs apparmor-unconfined). The hypervisor was reverted to stock: playground container removed, docker.io purged, /etc/apparmor.d/docker-default and /opt/nautilus-playground deleted, profile unloaded.

## Update 2: Cloudflare Tunnel + LAN-Isolation (2026-07-14 abends)
- cloudflared 2026.7.1 im LXC als systemd-Dienst mit Dashboard-Token; Tunnel HEALTHY (4 QUIC-Verbindungen, txl/fra, über IPv6 — DS-Lite umgangen).
- API-Container läuft jetzt mit TRUST_PROXY=1 (Per-IP-Rate-Limit hinter dem Tunnel via X-Forwarded-For).
- PVE-Firewall aktiv: cluster.fw (enable, Mgmt-Zugriff 22/8006 aus LAN explizit erlaubt) + 101.fw (OUT-Reject auf RFC1918, lokales v6-GUA-Prefix, ULA, fe80; DNS/DHCP zur Fritzbox erlaubt; policy_in ACCEPT). net0 von CT 101 mit firewall=1.
- Stolperfalle: Docker-Purge auf dem Node hatte FORWARD-Policy DROP hinterlassen; pve-firewall reaktivierte bridge-nf-call-iptables → aller LXC-Traffic tot. Fix: iptables -P FORWARD ACCEPT (nach Reboot ohnehin Standard, Docker ist weg).
- Verifiziert: Internet/DNS/v6 aus LXC ok, LAN (v4+v6) aus LXC geblockt, LAN→8080 weiter ok, E2E-Compile done/8 Stages, cloudflared aktiv, andere Gäste unberührt.
- Offen: Public-Hostname im Cloudflare-Dashboard (Schritt "Public Hostname" → HTTP localhost:8080), dann HTTPS-Test von außen.

## Update 3 (2026-07-15): öffentlich unter https://nautilus.grulich.me
- grulich.me-Zone zu Cloudflare migriert (jacob/maisie.ns.cloudflare.com); Website (GitHub Pages, jetzt proxied), Strato-MX und Google-TXT verifiziert intakt.
- Public Hostname nautilus.grulich.me → HTTP localhost:8080 im Tunnel; erreichbar über IPv4+IPv6 am CF-Edge trotz DS-Lite.
- E2E über die öffentliche URL verifiziert: frischer Compile-Job done, 8 Stages, sandbox 1.05 s; Result-Cache greift (identische Quelle → jobId "cached" mit Inline-Result).

# Ship Checklist — Nautilus Playground → Proxmox 192.168.178.32

Type: deployment. Target: `tools/playground` (branch `claude/nautilus-demo-ui-design-koqpl8`, commit `fc113f43`).

| Item | Result |
|---|---|
| Source committed & pushed | PASS — clean tree (only untracked `autoresearch/`), branch matches origin |
| Host reachable via SSH | PASS — Proxmox VE 9.2.3, Debian 13, x86_64, 12 cores, 53 GB free |
| Docker available | FIXED — installed `docker.io` 26.1.5 (was absent) |
| Runner image builds | PASS — `-j4` cap to protect VM RAM; ~5 min |
| API image builds | FIXED — AppArmor 4.x denied npm's unix socketpair; corrected `docker-default` profile installed |
| No secrets in deployment | PASS — no env secrets; docker.sock mount is by design (see compose SECURITY NOTE) |
| Service starts & stays up | PASS — `--restart unless-stopped`, `--init` |
| Web UI reachable from LAN | PASS — `GET /` 200, JS bundle 200 (4.3 MB) |
| API functional end-to-end | FIXED then PASS — job-dir `/out` unwritable by non-root runner on stock Linux daemon; `chmod 0o777` fix in `dockerRunner.ts` |
| Smoke suite (all 7 backends + negative fixtures) | PASS — `run_smoke.sh` all green (needed same chmod fix) |
| Rollback plan | `docker rm -f nautilus-playground` restores prior state; `apt-get purge docker.io` + delete `/etc/apparmor.d/docker-default` + `/opt/nautilus-playground` for full revert |

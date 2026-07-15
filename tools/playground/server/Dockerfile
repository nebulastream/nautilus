# Nautilus Playground API server image (builds the web app in too).
#
# Build from the repository root:
#   docker build -f tools/playground/server/Dockerfile -t nautilus-playground-api .
#
# The API container needs a container runtime socket to spawn sandboxed
# runner containers (see docker-compose.yml). It never compiles user code
# itself.

# Keep the repo layout so the web app's relative import of the IR parser
# (../../vscode-nautilus-ir/src) resolves unchanged.
FROM node:20-slim AS web-build
COPY tools/vscode-nautilus-ir /repo/tools/vscode-nautilus-ir
COPY tools/playground/web /repo/tools/playground/web
WORKDIR /repo/tools/playground/web
RUN npm install && npm run build

FROM node:20-slim AS server-build
WORKDIR /app
COPY tools/playground/server /app
RUN npm install && npm run build

FROM node:20-slim
# docker CLI so the server can spawn runner containers via the mounted socket.
RUN apt-get update && apt-get install -y --no-install-recommends docker.io \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY --from=server-build /app/package.json /app/package-lock.json* /app/
RUN npm install --omit=dev
COPY --from=server-build /app/dist /app/dist
COPY --from=web-build /repo/tools/playground/web/dist /app/web-dist
ENV WEB_DIST=/app/web-dist \
    PORT=8080
EXPOSE 8080
CMD ["node", "dist/index.js"]

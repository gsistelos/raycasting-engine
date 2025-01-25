FROM mcr.microsoft.com/vscode/devcontainers/base

RUN apt-get update && apt-get install -y \
    gh \
    cmake \
    clangd \
    libwayland-dev \
    libxkbcommon-dev \
    xorg-dev \
    && rm -rf /var/lib/apt/lists/*

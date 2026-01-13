FROM ubuntu:22.04

# Install Clang and essential build tools
RUN apt-get update && apt-get install -y \
	clang \
	llvm-15 llvm-15-tools \
	build-essential \
	libicu-dev \
	cmake \
	gdb \
	nano \
	git \
	&& rm -rf /var/lib/apt/lists/*

# Create a working directory
WORKDIR /usr/src/app

# Default command: open interactive shell
CMD ["/bin/bash"]

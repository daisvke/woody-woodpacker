FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y \
	zsh \
	curl \
	git \
	build-essential \
	bsdextrautils \
	gdb \
	valgrind \
	file \
	vim \
	nasm \
	&& rm -rf /var/lib/apt/lists/*

# Install Oh My Zsh
RUN sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" --unattended

# Make zsh default
RUN chsh -s /usr/bin/zsh root

WORKDIR /shared

CMD ["/usr/bin/zsh"]

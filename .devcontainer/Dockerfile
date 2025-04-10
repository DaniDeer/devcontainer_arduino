ARG VARIANT
FROM mcr.microsoft.com/devcontainers/cpp:${VARIANT}

ARG ARDUINO_CLI_VERSION
ARG USER_NAME

RUN apt update \
    && apt install -y \
    git \
    udev \
    bash-completion

# Adds the user to the group of which the group id corresponds to the host 'dialout' group id.
# If the group id doesn't exists locally, create a new group with the right group id and add the user to it.
# Add also the 'll' alias for the user and bash completion for git.
COPY ./envfile /root/
RUN . /root/envfile \
    && rm -f /root/envfile \
    && echo "alias ll='ls -ltr'" >> /home/$USER_NAME/.bashrc \
    && echo ". /usr/share/bash-completion/completions/git" >> /home/vscode/.bashrc \
    && if grep :${DIALOUT_GID}: /etc/group; then \
        usermod -a -G $(grep :${DIALOUT_GID}: /etc/group | cut -d ':' -f 1) $USER_NAME; \
    else \
        groupadd -g ${DIALOUT_GID} vscodedialout && usermod -a -G vscodedialout $USER_NAME; \
    fi

# Install Arduino CLI
RUN mkdir -p /opt/arduino-cli \
    && cd /opt/arduino-cli \
    && wget https://github.com/arduino/arduino-cli/releases/download/v${ARDUINO_CLI_VERSION}/arduino-cli_${ARDUINO_CLI_VERSION}_Linux_64bit.tar.gz \
    && tar -xf arduino-cli_${ARDUINO_CLI_VERSION}_Linux_64bit.tar.gz \
    && rm -f arduino-cli_${ARDUINO_CLI_VERSION}_Linux_64bit.tar.gz \
    && ln -s /opt/arduino-cli/arduino-cli /usr/local/bin/arduino-cli

# Set the default user
USER $USER_NAME

#LABEL arduino-ide-version="${ARDUINO_IDE_VERSION}"
LABEL arduino-cli-version="${ARDUINO_CLI_VERSION}"
LABEL desccription="Image based on Debuan providing Arduino build and debug environment"

# Project specific section
# ========================

# Install any extern libraries here. Example:
# Install LiquidCrystal I2C library
# RUN arduino-cli lib install "LiquidCrystal I2C"

# Install ESP8266 platform package
ARG PACKAGE_URL
ARG PLATFORM

RUN mkdir -p /home/$USER_NAME/.arduino15 \
    && arduino-cli config init \
    && arduino-cli config set board_manager.additional_urls $PACKAGE_URL \
    && arduino-cli core update-index \
    && arduino-cli core install $PLATFORM
# Install any extern libraries here. 
# See docu: https://arduino.github.io/arduino-cli/0.21/commands/arduino-cli_lib/
# https://arduino.github.io/arduino-cli/0.21/commands/arduino-cli_lib_install/
# Example:
# Install LiquidCrystal I2C library
# RUN arduino-cli lib install "LiquidCrystal I2C"
# && arduino-cli lib install ...
# arduino-cli lib install --config-file /home/$USER_NAME/.arduino15/arduino-cli.yaml \
#     --additional-urls $PACKAGE_URL \
    
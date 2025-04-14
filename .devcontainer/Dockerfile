ARG VERSION=1.2.9
ARG VARIANT=debian-12
# See tag list: https://mcr.microsoft.com/v2/devcontainers/cpp/tags/list
FROM mcr.microsoft.com/devcontainers/cpp:${VERSION}-${VARIANT}

ARG USER_NAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID
ARG ARDUINO_CLI_VERSION

# Install dependencies for Arduino CLI
#   python3, python3-pip: required by ESP8266 tools and potentially other libraries
#   python3-serial: often needed for upload tools (esp Pyserial)
#   udev: required for serial port access
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    curl \
    python3 \
    python3-pip \
    python3-serial \
    sudo \
    tar \
    udev \
    wget \
  && apt-get autoremove -y && apt-get clean -y && rm -rf /var/lib/apt/lists/*

# Install and configure hadolint
ARG HADOLINT_VERSION=2.12.0
RUN curl -L -o /usr/local/bin/hadolint https://github.com/hadolint/hadolint/releases/download/v${HADOLINT_VERSION}/hadolint-Linux-x86_64 \
    && chmod +x /usr/local/bin/hadolint

SHELL ["/bin/bash", "-o", "pipefail", "-c"]

# Install Arduino CLI
RUN curl -fsSL https://downloads.arduino.cc/arduino-cli/arduino-cli_${ARDUINO_CLI_VERSION}_Linux_64bit.tar.gz -o /tmp/arduino-cli.tar.gz \
  && tar -zxf /tmp/arduino-cli.tar.gz -C /usr/local/bin \
  && rm /tmp/arduino-cli.tar.gz \
  # Verify installation
  && arduino-cli version 

# Setup environment and user groups using the host's dialout GID
# Copy the envfile created by initializeCommand
COPY --chown=$USER_NAME:$USER_NAME ./envfile /tmp/envfile
# Source the envfile to get the environment variables.
# Adds the user to the group of which the group id corresponds to the host 'dialout' group id.
# If the group id doesn't exists locally, create a new group with the right group id and add the user to it.

RUN echo "Sourcing envfile to get environment variables." \
  # Source the envfile to get the environment variables
  && . /tmp/envfile \
  && rm -f /tmp/envfile \
  # Check if GID exists, add user to existing group or create new groupt with that GID
  && if getent group ${DIALOUT_GID} > /dev/null; then \
    EXISTING_GROUP_NAME=$(getent group ${DIALOUT_GID} | cut -d ':' -f 1); \
    echo "Adding user ${USER_NAME} to existing group ${EXISTING_GROUP_NAME} (GID ${DIALOUT_GID})." ;\
    usermod -a -G ${EXISTING_GROUP_NAME} ${USER_NAME}; \
  else \
    NEW_GROUP_NAME=vscodedialout; \
    echo "Creating new group ${NEW_GROUP_NAME} with GID ${DIALOUT_GID} and adding user ${USER_NAME}." ;\
    groupadd -g ${DIALOUT_GID} ${NEW_GROUP_NAME} && usermod -a -G ${NEW_GROUP_NAME} ${USER_NAME}; \
  fi

LABEL arduino-cli-version="${ARDUINO_CLI_VERSION}"
LABEL desccription="Image based on Debuan providing Arduino build and debug environment"

# Adds a non-root user with sudo access.
# Use the "remoteUser" property in devcontainer.json to use it.
# The container user GID/UIDs will be updated to match the host user GID/UIDs.
# See here: https://code.visualstudio.com/remote/advancedcontainers/add-nonroot-user

# Option 1: Create the user (if you use a base image that is not a devcontainer image)
# RUN groupadd --gid $USER_GID $USER_NAME \
#     && useradd --uid $USER_UID --gid $USER_GID -m $USER_NAME \
#     && echo $USER_NAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USER_NAME \
#     && chmod 0440 /etc/sudoers.d/$USER_NAME

# Option 2: If you use a devcontainer base image "vscode" group and user already exist
# Just add the user vscode to the sudoers file
RUN echo $USER_NAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USER_NAME \
    && chmod 0440 /etc/sudoers.d/$USER_NAME

# Install the rest as user
USER $USER_NAME

# Initialize Arduino CLI configuration, install cores, and update index
# Run as root initially to set up system-wide config and cores
# '--config-file' ensures we use a predictable location
# '--additional-urls' adds the ESP8266 board manager source
ARG PACKAGE_URL
ARG PLATFORM

# Configure Arduino CLI and install cores
RUN mkdir -p /home/${USER_NAME}/.arduino15 \ 
  && arduino-cli config init --overwrite --config-file /home/${USER_NAME}/.arduino15/arduino-cli.yaml \
  && arduino-cli config set --config-file /home/${USER_NAME}/.arduino15/arduino-cli.yaml board_manager.additional_urls ${PACKAGE_URL} \
  && arduino-cli core update-index --config-file /home/${USER_NAME}/.arduino15/arduino-cli.yaml \
  # Install standard Arduino AVR core to .arduino15/packages/arduino
  && arduino-cli core install arduino:avr --config-file /home/${USER_NAME}/.arduino15/arduino-cli.yaml \
  # Install ESP8266 core to .arduino15/packages/esp8266
  && arduino-cli core install ${PLATFORM} --config-file /home/${USER_NAME}/.arduino15/arduino-cli.yaml

# Project specific section
# ========================
# See docu: https://arduino.github.io/arduino-cli/0.21/commands/arduino-cli_lib/
# https://arduino.github.io/arduino-cli/0.21/commands/arduino-cli_lib_install/

# Optional: Pre-install common libraries if desired:
# Set the sketchbook folder
ENV SKETCHBOOK_DIR=/home/${USER_NAME}/Arduino

# Create the directory and install a standard lib
RUN mkdir -p ${SKETCHBOOK_DIR}/libraries \
    #&& arduino-cli lib install "LiquidCrystal I2C"
    # Clone your custom library to the correct path
    && git clone --depth 1 --branch 0.0.1 https://github.com/DaniDeer/FuncyControllerCPP.git \
      ${SKETCHBOOK_DIR}/libraries/FuncyControllerCPP

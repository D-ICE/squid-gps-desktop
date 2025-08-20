FROM debian:11.11

ARG DEBIAN_FRONTEND=noninteractive

RUN apt -y update && \
  # General dependencies
  apt -y install git build-essential python3 python3-pip python3-requests uuid-dev openssh-client \
    doxygen awscli \
  # HMI-QT dependencies
    libusb-dev libxtst-dev libxrandr-dev libxt-dev libxdamage-dev

RUN pip3 install cmake==3.25.0
RUN mkdir code
COPY . code

ARG BUILDDIR=build

WORKDIR code
RUN mkdir ${BUILDDIR}
WORKDIR ${BUILDDIR}
RUN cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${BUILDDIR}/bin -Dsquid_gps_enable_gui=OFF ..
RUN make -j6
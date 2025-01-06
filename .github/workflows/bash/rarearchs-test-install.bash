set -x
start_time=$SECONDS
time apt-get update -y
time apt-get install -y \
  git \
  build-essential
# arm platforms need an up-to-date cmake:
# https://gitlab.kitware.com/cmake/cmake/-/issues/20568
if [ "${{matrix.arch}}" == "armv6" ] || [ "${{matrix.arch}}" == "armv7" ] ; then
  time apt-get install -y \
    gpg \
    wget \
    apt-transport-https
  wget --no-check-certificate -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null
  echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ focal main' | tee /etc/apt/sources.list.d/kitware.list >/dev/null
  time apt-get update -y
  rm /usr/share/keyrings/kitware-archive-keyring.gpg
  time apt-get install kitware-archive-keyring
  time apt-get update -y
fi
time apt-get install -y cmake cmake-data
cmake --version
echo "install took $((SECONDS - start_time))"

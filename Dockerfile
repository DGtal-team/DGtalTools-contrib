# DGtalTools-contrib  based on the prebuilt DGtal and DGtalTools image
FROM docker.io/ker69/dgtaltools:latest



# Build DGtalTools-contrib
RUN git clone --depth 1 https://github.com/DGtal-team/DGtalTools-contrib.git && \
    cd DGtalTools-contrib && \
    cmake -S . -B build \
        -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build  && \
    cmake --install build




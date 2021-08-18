build:
    (mkdir -p build && \
    cd build && \
    qmake ../src && \
    make)

run:
    EDS_PATH=$PWD/eds ./bin/udtstudio
#!/bin/sh

set -e
VERSION=5.0.0rc0
U=$(id -u)
G=$(id -g)

many1_run() {
    docker run -v $PWD:$PWD -v $HOME/.cache/pip:/.cache/pip -u $U:$G -w $PWD -it quay.io/pypa/manylinux1_x86_64 "$@"
}
many2014_run() {
    docker run -v $PWD:$PWD -v $HOME/.cache/pip:/.cache/pip -u $U:$G -w $PWD -it quay.io/pypa/manylinux2014_x86_64 "$@"
}
many224_run() {
    docker run -v $PWD:$PWD -v $HOME/.cache/pip:/.cache/pip -u $U:$G -w $PWD -it quay.io/pypa/manylinux_2_24_x86_64 "$@"
}

python setup.py clean || true
rm -rf *.whl dist/* py/pocketsphinx.egg-info
python -m build --sdist
docker pull quay.io/pypa/manylinux1_x86_64
for version in cp39-cp39 cp38-cp38 cp37-cp37m; do
    many1_run /opt/python/$version/bin/pip wheel dist/pocketsphinx5-$VERSION.tar.gz
done
# Currently BROKEN: see https://github.com/pypa/manylinux/issues/1347
# docker pull quay.io/pypa/manylinux2014_x86_64
# many2014_run /opt/python/cp310-cp310/bin/pip wheel dist/pocketsphinx5-$VERSION.tar.gz
for w in *.whl; do
    many1_run auditwheel repair -w dist $w
    rm $w
done

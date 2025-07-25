# Build Instructions for cvc5 with MiniSatUP

> changes overview:
> - auto download and build MiniSatUP in cvc5
> - add `--sat-solver=minisatup` option in cvc5

## 1. Clone cvc5 from development branch

```
git clone --depth 1 --branch minisatup-ipasirup https://github.com/hchenqide/cvc5.git
```

## 2. Configure and build cvc5

```
cd cvc5

# cvc5$
./configure.sh debug --auto-download
# change debug to production in release mode

cd build

# cvc5/build$
make -j$(( $(nproc) + 1 ))

# (optional) make check
make check > check.out
```

> note: MiniSatUP will be downloaded and built from https://github.com/hchenqide/minisat.git, branch `cvc5-ipasirup` (cvc5/cmake/FindMiniSatUP.cmake)

### 3. Test with cvc5 binary

```
cvc5/build/bin/cvc5 --show-config
```

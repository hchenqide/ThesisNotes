# Build Instructions for cvc5 with MinisatUP

## 1. In the same parent folder, clone minisatup and cvc5 from development branches

```
git clone --depth 1 --branch cvc5-ipasirup https://github.com/hchenqide/minisat.git

git clone --depth 1 --branch minisatup-ipasirup https://github.com/hchenqide/cvc5.git
```

> note: in this version of cvc5, both options `--sat-solver=minisat` and `--sat-solver=cadical` will actually use `minisatup`

## 2. Build MinisatUP and cvc5

### Debug mode

**MinisatUP**

```
cd minisat

# minisat$
mkdir build; cd build

# minisat/build$
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
cmake --install . --prefix ./debug/install
```

**cvc5**

```
cd cvc5

# cvc5$
./configure.sh debug --auto-download --prefix=./install --dep-path="../minisat/build/debug/install;"
cd build

# cvc5/build$
make -j$(( $(nproc) + 1 ))

# (optional) make check
make check > check.out
```

### Release mode

> simply change build types from debug to release(production)

**MinisatUP**

```
cd minisat

# minisat$
mkdir build; cd build

# minisat/build$
cmake -DCMAKE_BUILD_TYPE=Release ..
make
cmake --install . --prefix ./release/install
```

**cvc5**

```
cd cvc5

# cvc5$
./configure.sh production --auto-download --prefix=./install --dep-path="../minisat/build/release/install;"
cd build

# cvc5/build$
make -j$(( $(nproc) + 1 ))

# (optional) make check
make check > check.out
```

> note: this configure option `--dep-path="../minisat/build/debug/install;"` only works when cvc5 and minisat are in the same parent folder, otherwise it should be pointed to the actual folder

### 3. Test with cvc5 binary

```
cvc5/build/bin/cvc5 --show-config
```

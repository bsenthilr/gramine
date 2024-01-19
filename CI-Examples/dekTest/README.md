# user key based data sealing

This example demonstrates the following flow:
1. User generates DEK - data encryption key
2. User seals the DEK with MRSIGNER
3. The DEK creates by user is used to encrypt the application "/data" path

# Building

## Building for Linux

Run `make` (non-debug) or `make DEBUG=1` (debug) in the directory.

## Building for SGX

Run `make SGX=1` (non-debug) or `make SGX=1 DEBUG=1` (debug) in the directory.

# Run with Gramine
With SGX:
```sh
gramine-sgx dek
```

# generate DEK key
```sh
gramine-sgx-pf-crypt gen-key  -w /tmp/pfkey
```

# Verify decryption
```sh
 gramine-sgx-pf-crypt decrypt -w $PWD/test_dek -i $PWD/data/test_data -o /tmp/dec
```

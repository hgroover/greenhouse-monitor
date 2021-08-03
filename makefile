# Build encoded or signed files from txt
# With signing (sreq output) the txt is not limited to small script snippets but can be any arbitrarily large binary.

# Legacy public key for signing encrypted requests. From poll-cmd 1.02 onwards, no longer used
INKEY:=my-pi-pub.pem

# Private / public key pair for signing executables. Used in poll-cmd 1.02 onwards.
# PRIVATE_SIGNING should reside only on the secure trusted machine used to issue signed requests.
# PUBLIC_SIGNING_VERIFY must reside on any remote pi that needs to verify signatures.
# As with domain certificates, it can be safely distributed since there's no way to sign
# requests using the public key, only to verify.
# To generate the private key: openssl genrsa -out my-pi-signing-private.pem 4096
# (keep in a safe place and do not distribute)
# To generate the public key:  openssl rsa -in my-pi-signing-private.pem -pubout -out my-pi-signing-public.pem
# (copy public key to any remote pis which will verify and execute signed packages)

PRIVATE_SIGNING:=my-pi-signing-private.pem
PUBLIC_SIGNING_VERIFY:=my-pi-signing-public.pem

# Actual config should be set in your include file
-include signing-config.inc

%.enc : %.txt
	openssl rsautl -encrypt -inkey ${INKEY} -pubin -in $< -out $@

%.sig : %.txt
	openssl dgst -sha256 -sign ${PRIVATE_SIGNING} -out $@ $<

# test for presence is needed only because pipeline masks failures in openssl
%.sreq : %.txt
	test "${PRIVATE_SIGNING}" -a -s ${PRIVATE_SIGNING}
	openssl dgst -sha256 -sign ${PRIVATE_SIGNING} $< | cat - $< > $@

%.srtest : %.sreq
	ls -l $< > $@
	head -c 512 $< > $@.sig
	tail -c +513 $< > $@.data
	openssl dgst -sha256 -verify ${PUBLIC_SIGNING_VERIFY} -signature $@.sig $@.data
	@echo "Signature verified"
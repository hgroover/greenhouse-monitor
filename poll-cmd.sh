#!/bin/bash
# Poll command - requires curl
SCRIPT_VER=1.02
# Original (unversioned) script required command to run in less than 5 minutes. Fixed in 1.02
# In 1.02, ${PUBKEY} is used to verify request was signed with sender's private key to allow scripts larger than 220 bytes
echo "$0 v${SCRIPT_VER}"
[ -x .cfg/cnc ] || { echo "No config"; exit 1; }
. .cfg/cnc
[ "${URL}" ] || { echo "No URL in config"; exit 1; }
[ "${DATADIR}" ] || { echo "No DATADIR in config"; exit 1; }
# For 1.02 signed requests, we also need a PUBKEY path. To generate a pair of keys for signature verification,
# see this: https://www.zimuel.it/blog/sign-and-verify-a-file-using-openssl
# Basically openssl genrsa -out gardenpi-signing-private.pem 4096
# then openssl rsa -in gardenpi-signing-private.pem -pubout -out gardenpi-signing-pub.pem
# The private key needs to be protected and the public key goes on the pi itself for verification.
# PUBKEY points to this public key. The number of bits corresponds to the binary size of the signature
# expressed as PUBKEY_BYTES (bits / 8, default = 4096 / 8 = 512)
[ "${PUBKEY}" ] || { echo "No PUBKEY in config"; exit 1; }
[ "${PUBKEY_BYTES}" ] || PUBKEY_BYTES=512
[ -r ${PUBKEY} ] || { echo "PUBKEY ${PUBKEY} is not readable"; ls -l ${PUBKEY}; exit 1; }
[ -r ${DATADIR}/last ] || { echo "${DATADIR}/last not found"; exit 1; }
LAST=$(cat ${DATADIR}/last)
echo "last=${LAST}, checking..."
TMPFILE=/tmp/pollcmd.$$
curl --get ${URL}/poll/${LAST} > ${TMPFILE}.next || { echo "curl failure"; rm -f ${TMPFILE}.*; exit 1; }
NEW=$(cat ${TMPFILE}.next)
echo "From ${TMPFILE}.next: ${NEW}"
if [ "${NEW}" != "" -a 0${NEW} -gt 0 ]
then
  echo "New request ${NEW}"
  curl --get ${URL}/get/${NEW} > ${TMPFILE}.req || { echo "request fetch failure"; ls -l ${TMPFILE}.*; rm -f ${TMPFILE}.*; exit 1; }
  # Immediately record updated last fetched.
  echo "${NEW}" > ${DATADIR}/last
  echo "Last updated. Download:"
  ls -l ${TMPFILE}.req
  if [ -s ${TMPFILE}.req ]
  then
    # Remove ${PUBKEY_BYTES} from the head. tail -c +<start-byte> takes the origin:1 address of the starting byte
	head -c ${PUBKEY_BYTES} ${TMPFILE}.req > ${TMPFILE}.sig
	TAILBYTES=$(expr ${PUBKEY_BYTES} \+ 1)
	tail -c +${TAILBYTES} ${TMPFILE}.req > ${TMPFILE}.cmd
	# Verify signature
	if openssl dgst -sha256 -verify ${PUBKEY} -signature ${TMPFILE}.sig ${TMPFILE}.cmd
	then
	  echo "Signature verified, executing"
	  ls -l ${TMPFILE}.cmd
	  chmod +x ${TMPFILE}.cmd
	  echo " -- Runner $0 v${SCRIPT_VER}" > ${DATADIR}/${NEW}.out
	  REQNUM=${NEW} ${TMPFILE}.cmd >> ${DATADIR}/${NEW}.out 2>&1
	  echo " -- Result: $?" >> ${DATADIR}/${NEW}.out
	  curl -F "req_out=@${DATADIR}/${NEW}.out" ${URL}/put/${NEW}
	else
	  echo "Signature failure"
	fi
  else
    echo "Empty request"
  fi
else
  echo "Nothing new"
fi
rm -f ${TMPFILE}*

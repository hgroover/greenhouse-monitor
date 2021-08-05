#!/bin/bash
# Fetch script from github and make it executable
SCRIPT_DIR=/home/pi/github-scripts
URL_BASE=https://github.com
URL_USER=hgroover
URL_PROJECT=greenhouse-monitor
URL_BRANCH=master

# To support self-updates, copy to /tmp and exec
MYDIR=$(dirname $0)
echo "Path $0 dir ${MYDIR}"
if [ "${MYDIR}" = "/tmp" ]
then
  echo "Running from /tmp"

  # If none specified, print usage and exit
  [ "$1" ] || { echo "Syntax: $0 <script-path1> [<script-path2> ...]"; exit 1; }
  [ -d ${SCRIPT_DIR} ] || { echo "Creating ${SCRIPT_DIR}"; mkdir -p ${SCRIPT_DIR}; }

  cd ${SCRIPT_DIR}
  URL=${URL_BASE}/${URL_USER}/${URL_PROJECT}/raw/${URL_BRANCH}
  FETCH_COUNT=0
  UPDATE_COUNT=0
  while [ "$1" ]
  do
    FDIR=$(dirname $1)
	[ -d ${FDIR} ] || { echo "Creating ${FDIR}"; mkdir -p ${FDIR}; }
    wget --quiet ${URL}/$1 -O $1.__tmp || { echo "Failed to fetch $1 from ${URL}"; exit 1; }
    FETCH_COUNT=$(expr ${FETCH_COUNT} + 1)
    chmod +x $1.__tmp
	# Is it new?
	if [ -r $1 ]
	then
	  # Is it changed?
	  if diff $1 $1.__tmp > /dev/null
	  then
	    echo "No change in $1"
		rm -f $1.__tmp
	  else
	    echo "Updating $1"
		mv -f $1.__tmp $1
		UPDATE_COUNT=$(expr ${UPDATE_COUNT} + 1)
	  fi
	else
	  echo "New script $1"
	  mv $1.__tmp $1
	  UPDATE_COUNT=$(expr ${UPDATE_COUNT} + 1)
	fi
    shift
  done

  echo "${FETCH_COUNT} scripts downloaded, ${UPDATE_COUNT} updated"

else
  MYBASE=$(basename $0)
  cp -p $0 /tmp/${MYBASE}
  exec /tmp/${MYBASE} $*
fi


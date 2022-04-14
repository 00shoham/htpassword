#!/bin/sh

ZZ=`cat config.ini|grep "^#exit"`
if [ -z "$ZZ" ] ; then
  echo "Config OK - does not exit"
else
  echo "Config file [config.ini] is setup for debug - fix it first!"
  exit 1
fi

sudo install -o root -g root -m 755 config.ini /usr/local/etc/htpassword.ini


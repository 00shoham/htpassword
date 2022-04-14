#!/bin/sh

#make clean
make

BIN="htpassword"

./config-test || exit

CGIDIR=/data/cgi-bin
WEBDIR=/data/www
TWEBDIR=$WEBDIR/htpassword
ETCDIR=/data/etc
LOGDIR=/var/log/htpassword
WEBUSER=www-data

if [ -d "$LOGDIR" ] ; then
  echo "$LOGDIR already exists"
else
  sudo mkdir "$LOGDIR"
  sudo chown $WEBUSER.$WEBUSER "$LOGDIR"
  sudo chown 755 "$LOGDIR"
fi

if [ -d "$CGIDIR" ] ; then
  echo "$CGIDIR already exists"
else
  sudo mkdir "$CGIDIR"
  sudo chown root.root "$CGIDIR"
  sudo chown 755 "$CGIDIR"
fi

if [ -d "$WEBDIR" ] ; then
  echo "$WEBDIR already exists"
else
  sudo mkdir "$WEBDIR"
  sudo chown root.root "$WEBDIR"
  sudo chown 755 "$WEBDIR"
fi

if [ -d "$TWEBDIR" ] ; then
  echo "$TWEBDIR already exists"
else
  sudo mkdir "$TWEBDIR"
  sudo chown root.root "$TWEBDIR"
  sudo chown 755 "$TWEBDIR"
fi

for b in $BIN; do
  if [ -f "$b" ]; then
    if [ -f "$CGIDIR/$b" ]; then
      sudo rm "$CGIDIR/$b"
    fi
    sudo install -o root -g root -m 755 "$b" "$CGIDIR"
  fi
done

for f in `cat config.ini | grep '^HISTORY_FILE='`; do
  ff=`echo $f | sed 's/.*=//'`
  sudo touch "$ff"
  sudo chown "$WEBUSER" "$ff"
  sudo chgrp "$WEBUSER" "$ff"
  sudo chmod 644 "$ff"
done

for f in `cat config.ini | grep '^LOCK_FILE='`; do
  ff=`echo $f | sed 's/.*=//'`
  sudo touch "$ff"
  sudo chown "$WEBUSER" "$ff"
  sudo chgrp "$WEBUSER" "$ff"
  sudo chmod 644 "$ff"
done


sudo install -o root -g root -m 644 ui.html $TWEBDIR
sudo install -o root -g root -m 644 ui.js $TWEBDIR
sudo install -o root -g root -m 644 ui.css $TWEBDIR

sh install-config.sh

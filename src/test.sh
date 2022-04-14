#!/bin/sh

sudo htpasswd -b /data/etc/sample.htpasswd idan '5*Hotel'
sudo bash -c "echo '' > /data/etc/password.history"

#curl\
#  -i\
#  --data '{"ID":"main", "USERID":"idan", "OLDP":"5*Hotel", "NEWP":"6*Hotel"}'\
#  'http://localhost/cgi-bin/htpassword?api/password/changepassword'\
#  --header 'Content-Type: application/json'\
#  --header 'Accept: application/json'\

#curl\
#  -i\
#  --data '{"ID":"main", "USERID":"idan", "PASSWORD":"6*Hotel"}'\
#  'http://localhost/cgi-bin/htpassword?api/password/resetpassword'\
#  --header 'Content-Type: application/json'\
#  --header 'Accept: application/json'\

#curl\
#  -i\
#  --data '{"ID":"main", "USERID":"idan2", "PASSWORD":"6*Hotel"}'\
#  'http://localhost/cgi-bin/htpassword?api/password/adduser'\
#  --header 'Content-Type: application/json'\
#  --header 'Accept: application/json'\
#
#curl\
#  -i\
#  --data '{"ID":"main", "USERID":"idan2"}'\
#  'http://localhost/cgi-bin/htpassword?api/password/deluser'\
#  --header 'Content-Type: application/json'\
#  --header 'Accept: application/json'\
#
#curl\
#  -i\
#  --data '{"ID":"main"}'\
#  'http://localhost/cgi-bin/htpassword?api/user/list'\
#  --header 'Content-Type: application/json'\
#  --header 'Accept: application/json'\
#
#sudo htpasswd -b /data/etc/sample.htpasswd idan '5*Hotel'
#sudo bash -c "echo '' > /data/etc/password.history"

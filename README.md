# htpassword

This is a simple, lightweight web UI for managing users and changing
passwords in Apache2-style .htpasswd files.

You can manage credentials in multiple files.

The configuration is found in /usr/local/etc/htpassword.ini and a sample
config.ini is provided.

You need a copy of 'idanlib' as the code in this project links against
that library.  Get that from git@github.com:00shoham/idanlib.git.
Don't forget to run 'make' there.

You also need lua to compile this.  It's actually not used by this
project but is a dependency of idanlib.

The UI is pretty basic but it does leverage a RESTful (JSON/HTTP) API.

The UI is just HTML + JS + CSS.  The "back end" is a simple CGI that
exposes the aforementioned API.  You do need to configure that in Apache2.
My config is just this:

Alias /htpassword/ "/data/www/htpassword/"
<Directory "/data/www/htpassword/">
  AuthType None
  Options Indexes FollowSymLinks
  AllowOverride None
  Require all granted
</Directory>

I access the UI at http://localhost/htpassword/ui.html

The folder /data/www/htpassword/ contains ui.html, ui.js and ui.css.

To enable the API/CGI:
  * In the folder /etc/apache2/mods.enabled, we have
     cgi.load -> ../mods-available/cgi.load

  * In the folder /etc/apache2/conf.enabled, we have
     serve-cgi-bin.conf -> ../conf-available/serve-cgi-bin.conf

  * In the file serve-cgi-bin.conf, we have:
    ScriptAlias /cgi-bin/ /data/cgi-bin/
    <Directory "/data/cgi-bin">
      AllowOverride None
      Options +ExecCGI -MultiViews +SymLinksIfOwnerMatch
      Require all granted
    </Directory>

  * In the folder /data/cgi-bin we install the htpassword binary.

That's about it.


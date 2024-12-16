@echo off
cscript /nologo /e:jscript configure.js  "--with-winbinder=shared" "--enable-debug" %*
